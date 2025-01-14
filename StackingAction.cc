//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file B3/B3a/src/StackingAction.cc
/// \brief Implementation of the B3::StackingAction class

#include "StackingAction.hh"

#include "G4NeutrinoE.hh"
#include "G4Track.hh"

#include <fstream> // Th�m th? vi?n ghi file

namespace B3
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ClassificationOfNewTrack StackingAction::ClassifyNewTrack(const G4Track* track)
{
  // Ghi th�ng tin h?t v�o file
  static std::ofstream outFile("stacking_data.txt", std::ios::app); // Ghi n?i ti?p

  if (track) {
        // L?y th�ng tin volume
        const G4VPhysicalVolume* volume = track->GetVolume();
        G4String volumeName = volume ? volume->GetName() : "Unknown";

        // Ghi th�ng tin v�o file
        outFile << "Track ID: " << track->GetTrackID()
            << ", Parent ID: " << track->GetParentID()
            << ", Particle: " << track->GetDefinition()->GetParticleName()
            << ", Energy: " << track->GetKineticEnergy() / keV << " keV"
            << ", Position: " << track->GetPosition() / mm << " mm"
            << ", Volume: " << volumeName
            << std::endl;
  }
  // keep primary particle
  if (track->GetParentID() == 0) return fUrgent;

  // kill secondary neutrino
  if (track->GetDefinition() == G4NeutrinoE::NeutrinoE())
    return fKill;
  else
    return fUrgent;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}  // namespace B3
