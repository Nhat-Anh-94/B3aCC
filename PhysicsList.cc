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

#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4Gamma.hh"

namespace B3
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsList::PhysicsList()
{
  SetVerboseLevel(1);
  // Khởi tạo các hạt (bao gồm gamma) trước khi thêm các quá trình
  ConstructParticle();
}

void PhysicsList::ConstructParticle()
{
    // Khởi tạo hạt gamma (nếu chưa được khởi tạo)
    G4Gamma::Gamma();
}

void PhysicsList::ConstructProcess()
{
    // Lấy danh sách hạt từ G4ParticleTable
    auto particleTable = G4ParticleTable::GetParticleTable();
    auto gamma = G4Gamma::Gamma();

    if (gamma)
    {
        auto processManager = gamma->GetProcessManager();
        if (!processManager)
        {
            G4Exception("PhysicsList::ConstructProcess", "NoProcessManager",
                FatalException, "G4Gamma does not have a process manager!");
        }

        // Xóa các quá trình cũ nếu có
        //processManager->RemoveAllProcesses();

        // Thêm quá trình hấp thụ quang điện
        auto photoelectricEffect = new G4PhotoElectricEffect();
        processManager->AddDiscreteProcess(photoelectricEffect);

        // Thêm quá trình tán xạ Compton
        auto comptonScattering = new G4ComptonScattering();
        processManager->AddDiscreteProcess(comptonScattering);
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::SetCuts()
{
  G4VUserPhysicsList::SetCuts();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}  // namespace B3
