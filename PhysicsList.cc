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
/// \file B3/B3a/src/PhysicsList.cc
/// \brief Implementation of the B3::PhysicsList class

#include "PhysicsList.hh"

#include "G4DecayPhysics.hh"
#include "G4EmStandardPhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"

//Compton scattering
#include "G4EmStandardPhysics_option1.hh"  // Chỉ thêm các tương tác EM cơ bản
#include "G4PhotoElectricEffect.hh"        // Hấp thụ quang điện
#include "G4ComptonScattering.hh"          // Tán xạ Compton
#include "G4ProcessManager.hh"             // Quản lý quá trình

namespace B3
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsList::PhysicsList()
{
  SetVerboseLevel(1);

  // Lấy đối tượng photon và quản lý quá trình của nó
  G4ParticleDefinition* gamma = G4Gamma::GammaDefinition();
  G4ProcessManager* pManager = gamma->GetProcessManager();

  // Thêm quá trình tán xạ Compton
  G4ComptonScattering* comptonEffect = new G4ComptonScattering();
  pManager->AddProcess(comptonEffect, 0, 2, 2);  // 2: after transportation

  // Thêm quá trình hấp thụ quang điện
  G4PhotoElectricEffect* photoEffect = new G4PhotoElectricEffect();
  pManager->AddProcess(photoEffect, 0, 1, 1);   // 1: before scattering

  // Chỉ thêm các quá trình EM cơ bản
  //RegisterPhysics(new G4EmStandardPhysics_option1());

  // Default physics
  //RegisterPhysics(new G4DecayPhysics());

  // EM physics
  //RegisterPhysics(new G4EmStandardPhysics());

  // Radioactive decay
  //RegisterPhysics(new G4RadioactiveDecayPhysics());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::SetCuts()
{
  G4VUserPhysicsList::SetCuts();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}  // namespace B3
