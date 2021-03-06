// File and Version Information:
// $Header: /nfs/slac/g/glast/ground/cvs/G4Generator/src/IntDetectorManager.cxx,v 1.29 2010/06/04 14:01:40 usher Exp $
//
// Description: This is a concrete implementation of the DetectorManager
// abstract class; this one is used to manage sensitive detectors of integrating
// type
//
// Author(s):
//      R.Giannitrapani

#include "IntDetectorManager.h"
#include "McParticleManager.h"
#include "McTrajectoryManager.h"

#include <iostream>
#include "Event/TopLevel/EventModel.h"
#include "Event/MonteCarlo/McIntegratingHit.h"
#include "Event/MonteCarlo/McParticle.h"
#include "idents/VolumeIdentifier.h"

#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Vector/ThreeVector.h"

// Geant4 interface
#include "G4Step.hh"
// #include "G4VSolid.hh"
// #include "G4Box.hh"
// #include "G4SDManager.hh"
#include "G4AffineTransform.hh"
#include "G4Box.hh"

#include <algorithm>

IntDetectorManager::IntDetectorManager(DetectorConstruction *det,
                                       IDataProviderSvc* esv, IGlastDetSvc* gsv)
  :DetectorManager(det->idMap(), esv, gsv, "IntegratingDetectorManager")
{
    // See the father class DetectorManager
    m_diodeGlueIndex = 1.46;  // From Eric Grove 10-Jun-2010
    m_crystalIndex   = 1.8;

    double criticalAngle = asin(m_diodeGlueIndex / m_crystalIndex);

    m_cosCritical = cos(criticalAngle);
}

void IntDetectorManager::Initialize(G4HCofThisEvent*)
{
    // Purpose and Method: initialization of the detectors manager
    // Inputs: the G4HCofThisEvent is hinerited from the Geant4 structure and is
    // of no use for our actual implementation of G4Generator (but must be there
    // for Geant4 internal working) 

    // get thr prefix from the GlastDetSvc; if the topvolume is equal to LAT this
    // prefix is empty
    m_prefix = m_gsv->getIDPrefix();

    // clear the list of hited detectors
    m_detectorList.clear();
    // At the start of the event we create a new container for the TDS
    m_intHit = new Event::McIntegratingHitVector;    

    //  m_table.init();
}

G4bool IntDetectorManager::ProcessHits(G4Step* aStep,
                                       G4TouchableHistory* ROhist)
{    
    // Purpose and Method: this method is called internally by Geant4 every time a
    // particle issue an hit in a sensitive detector of this kind
    // Inputs: the step of the hit and the hierarchy of touchable volumes
    // Outpus: false if there is no hit to register, true otherwise

  // If the nb of McIntegratingHit has hit the limit, stop processing hits
  if(McTrajectoryManager::getPointer()->m_counterMcIntegratingHit>McTrajectoryManager::getPointer()->m_maxMcIntegratingHit) return false;

    // Energy Deposition & Step Length  
    G4double edep = aStep->GetTotalEnergyDeposit()/MeV;
    G4double stepl = aStep->GetStepLength()/mm;
    
    if ((edep==0.)) return false;

    // Total energy summed below
    double totalDepE = 0.;

    // Physical Volume    
    G4TouchableHistory* theTouchable = (G4TouchableHistory*)(aStep->GetPreStepPoint()->GetTouchable());
    G4VPhysicalVolume*  physVol      = theTouchable->GetVolume(); 
    G4String            nameVolume   = physVol->GetName();
    G4LogicalVolume*    logVol       = physVol->GetLogicalVolume();
    G4String            material     = logVol->GetMaterial()->GetName();

    // start position of the hit and final one
    G4ThreeVector prePos = aStep->GetPreStepPoint()->GetPosition();
    G4ThreeVector postPos = aStep->GetPostStepPoint()->GetPosition();
    
    // determine the ID by studying the history, then call appropriate 
    idents::VolumeIdentifier id = constructId(aStep);

    // If the topvolume is not LAT, prepend a prefix in order to obtain a complete ID
    if (m_prefix.size() != 0) id.prepend(m_prefix);

    // We want to fill an integrating hit
    Event::McIntegratingHit *hit; 

    // If the hit has already been created we use it, otherwise we
    // create a new one
    if( !(hit = m_detectorList[id]))
    {
        // A new object is needed
        hit = new Event::McIntegratingHit;
        // Increment the McIntegratingHit counter to allow event abortion when the counter hits the limit
        McTrajectoryManager::getPointer()->m_counterMcIntegratingHit += 1;
        // Set its volume identifier
        hit->setVolumeID(id);
        // Put it in the collection of hits
        m_intHit->push_back(hit);
        // Keep track of already used id
        m_detectorList[id] = hit;
    }

    // map of diode volumes attached to this segment
    std::map<std::string,const G4VPhysicalVolume*> diodeMap;

    // *********************************************************************************************
    // Insert code section to try to model edge effects in Cal Crystals
    //
    // Search for positive and negative diodes... we'll need these for the direct light calculation
    // Of course, if we are already in the diodes then this is not necessary...
    if (nameVolume.contains("CsISeg"))
	{
        // Get the local to global transformation for the volume we are in
        G4AffineTransform localToGlobal(theTouchable->GetRotation(), theTouchable->GetTranslation());

        // Use the above, AND the transformation for the CsISeg to CsIDetector/Element frame
        G4ThreeVector localEntry = localToGlobal.Inverse().TransformPoint(aStep->GetPreStepPoint()->GetPosition())
                                 + physVol->GetTranslation();
        G4ThreeVector localExit  = localToGlobal.Inverse().TransformPoint(aStep->GetPostStepPoint()->GetPosition())
                                 + physVol->GetTranslation();

        // Vector between the entrance and exit points to get particle trajectory through crystal
        G4ThreeVector trackAxis  = localExit - localEntry;
        double        trackLen   = trackAxis.mag();

        // Normalize to turn into a track unit vector
        trackAxis.setMag(1.);

        // Now go through the touchable history depth to find the volume enclosing all pieces of the crystal 
        // (including the diodes)
        const G4VPhysicalVolume* volume = 0;

        // First loop up to find the enclosing CsIElement for this crystal
	    for( int i = 0; i<theTouchable->GetHistoryDepth() ; ++i) 
        {
	        volume = theTouchable->GetVolume(i); 
            G4String volName = volume->GetName();
            if (volName.contains("CsIElement")) break;
	    }

        // Provided we found the CsIElement, now look for the companion diodes
        // Of course, if we are in a diode then skip this
        if (volume->GetName().contains("CsIElement") && !physVol->GetName().contains("diode"))
        {
            const G4LogicalVolume* logVol = volume->GetLogicalVolume();

            // Loop over the daughters of the enclosing volume
            for(int idx = 0; idx < logVol->GetNoDaughters(); idx++)
            {
                const G4VPhysicalVolume* daughter = logVol->GetDaughter(idx);
            
                G4String volName = daughter->GetName();
            
                // Check to see if we have a diode volume
                if (volName.contains("diode"))
                {
                    // Retrieve the transform to center of diode face
                    G4ThreeVector diodePos = daughter->GetTranslation();
                            
                    // Retrieve the information giving the details of the diode volume
                    G4Box* diodeBox = dynamic_cast<G4Box*>(daughter->GetLogicalVolume()->GetSolid());

                    // From the box info, get the face area of this diode
                    double diodeFaceAlpha = 2. * diodeBox->GetYHalfLength();
                    double diodeFaceBeta  = 2. * diodeBox->GetZHalfLength();

                    // Make a unit normal pointing into the crystal, start by assuming 
                    // we are at the positive face 
                    G4ThreeVector xTalFaceNrm(-1.,0.,0.); 
                    G4String      endName = volName + "POS";
                    
                    // Swap if we are at the negative face
                    if (diodePos.x() < 0.)
                    {
                        endName = volName + "NEG";
                        xTalFaceNrm = G4ThreeVector(1.,0.,0.);
                    }

                    // Retrieve reference to object to fill for this diode
                    Event::McIntegratingHit::XtalEnergyDep& xtalDep = hit->getXtalEnergyDep(endName);

                    // Poor man's integration over trajectory of particle through crystal
                    // Divide into steps of 0.5 mm, though we need to make at least one step
                    int           nSteps     = 1; //std::max(1., 2. * trackLen);
                    G4ThreeVector stepPos    = localEntry + 0.5 * (trackLen / double(nSteps)) * trackAxis;
                    double        directFrac = 0.;
                    double        totalDep   = 0.;

                    for(int stepIdx = 0; stepIdx < nSteps; stepIdx++)
                    {
                        // Get a line from current position to diode face center
                        // and the distance to it
                        G4ThreeVector lineToCntr = stepPos - diodePos;
                        double        distToCntr = lineToCntr.mag();

                        // Find angle with normal which will be used to scale exposed face area of diode
                        double cosTheta = lineToCntr.unit().dot(xTalFaceNrm);

                        // Projected area of rectangle is diodeFaceArea * cosTheta, which can also be 
                        // written as diodeFaceAlpha * diodeFaceBeta * cosTheta, so we scale diodeFaceBeta
                        // by cosTheta for what follows
                        double diodeFaceBetaPr = diodeFaceBeta * cosTheta;

                        // Determine fractional solid angle subtended. For this problem, this is the solid
                        // angle subtended by a pyramid.
                        double radical   = (4.*distToCntr*distToCntr + diodeFaceAlpha*diodeFaceAlpha)
                                         * (4.*distToCntr*distToCntr + diodeFaceBetaPr*diodeFaceBetaPr);
                        double aSinArg   = diodeFaceAlpha * diodeFaceBetaPr / sqrt(radical);
                        double fracAngle = asin(aSinArg) / M_PI;

                        // Does angle to surface normal put us in the range of surface reflection? 
                        if (distToCntr < 30.) 
                        {
                            // Its clearly not right to only check angle to center of diode. To handle 
                            // reflection one should consider entire diode surface and develop some sort
                            // of transmission coefficient. But until then try to allow some transmission...
                            // So, idea here is to "transition" between the brewster angle and zero
                            //double angleCoeff = (m_cosCritical - cosTheta) / m_cosCritical;
							//double angleCoeff = std::max(0., (cosTheta - m_cosCritical) / (.1*m_cosCritical));
							//if(cosTheta > 1.15*m_cosCritical) angleCoeff = 1.;

                            // Attempt to attenuate the "direct" light...
                            //directFrac +=(angleCoeff * angleCoeff) * fracAngle;

                            // And now attenuate the deposited energy
						//	double thetaCrt = .428; //= acos(.91) .... was acos(1.15*m_cosCritical);
						//	double theta    = acos(cosTheta);
						//	double angleDiff = std::max(0., (theta - thetaCrt)/theta)/(3.141596/2.-thetaCrt);
                        //    totalDep   += 1.0 - angleDiff*angleDiff;// - 0.2* angleCoeff; // was .4*...
						//	double attenFactor = (30. - distToCntr)/30.;
						//	totalDep += 1. - .25*attenFactor*attenFactor; 

						// The following is based on patterns extracted from flight data
							double dist2Side = std::max(0., 13.5 -fabs(stepPos.y()));
							double dist2End   = std::max(0., 165.-fabs(stepPos.x()));

						// No angle factor for direct light - probably due to end roughening...
							directFrac += fracAngle;

						// Total light attenuation governed by location - strong dependence side to side, 
						// as well as dependencce with distance from end
							if(dist2End < 30) {
								double attenFactor = ((30. - dist2End) * (std::max(0., 13.5-dist2Side)))/ (30.*13.5);
								totalDep += 1. - .33*attenFactor * attenFactor;
							}
							else {
								totalDep += 1.;
								directFrac += fracAngle;
							}
                        }
                        // Otherwise, all light "seen" by diode and all energy deposited in crystal
                        else
                        {
                            directFrac += fracAngle;
                            totalDep   += 1.;
                        }

                        // Take the step
                        stepPos += (trackLen / double(nSteps)) * trackAxis;
                    }

                    // Now get the direct and total energy fractions
                    double directDepE = edep * directFrac / double(nSteps);
                    double totalDepE  = edep * totalDep / double(nSteps);

                    xtalDep.addEnergyItem(totalDepE, directDepE, (localExit+localEntry)/2);
                }
            }
        }
        // Otherwise, we are in a diode volume! 
        else
        {
        }
	}
    // End of attempt at edge effects 
    // *********************************************************************************************

    // this rotates the hit to local coordinates with respect to the center  
    CLHEP::HepRotation local(*(theTouchable->GetRotation()));
    HepPoint3D center=theTouchable->GetTranslation();

    prePos  = local * (prePos-center);
    postPos = local * (postPos-center);

    Event::McIntegratingHit::Particle p;

    McParticleManager* partMan = McParticleManager::getPointer();

    // Retrieve the kind of origin particle
    if (partMan->getOriginParticle()->mother().primaryParticle())
        p = Event::McIntegratingHit::primary;
    else
        if (partMan->getOriginParticle()->particleProperty() == 11)
            p = Event::McIntegratingHit::electron;
    else p = Event::McIntegratingHit::positron;

    // fill the energy and position    
    hit->addEnergyItem(edep, p, (prePos+postPos)/2);

    // Fill the ID array?
    // Note that this will be the PDG id, not the "std" id used in the beginning
    int particlePDGId = aStep->GetTrack()->GetDefinition()->GetPDGEncoding();

    // Heavy ions return a zero for ID, if we are not pruning then can look up from McParticle
    Event::McParticle* mcPart = partMan->getLastParticle();

    // This now sets the ID to the "std" HEP id...
    if (mcPart) particlePDGId = mcPart->particleProperty();

    // Creat the map entry and add to list
    std::pair<Event::McParticle::StdHepId, double> idEnePair(particlePDGId,edep);
    hit->itemizedEnergyId().push_back(idEnePair);

    // Add relation if McParticle exists
    if (mcPart) partMan->addMcRelation(mcPart, hit);

    // This method is always called BEFORE the SteppingAction where the trajectory
    // hits are stored. Copy the pointer to the McTrajectoryManager so we can have it
    // to store with the hit
    McTrajectoryManager::getPointer()->setMcIntHit(hit);

    return true;
}

void IntDetectorManager::EndOfEvent(G4HCofThisEvent*)
{
    // Purpose and Method: this method finalize the manager by saving the
    // integrating hits collection in the TDS
    // Inputs: again G4HCofThisEvent is used only internally by Geant4 and is of
    // no use for us
    // TDS Outputs: The collection of McIntegratingHit is saved in the 
    // /Event/MC/IntegratingHitsCol folder
  
    // Let's sort the hits by volume identifier
    std::sort(m_intHit->begin(),m_intHit->end(), Event::CompareIntHits());

    // store the hits in the TDS
    m_esv->registerObject(EventModel::MC::McIntegratingHitCol , m_intHit);    

#if 0
    // This message is for debug purpouses .. it should be eliminated or converted
    // to a true GAUDI log message
    std::cout << "Actual Event done! " << m_intHit->size() 
              << " integrating hits stored in the TDS" << std::endl;
#endif
}
