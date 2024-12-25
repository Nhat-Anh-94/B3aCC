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
/// \file B3/B3a/src/DetectorConstruction.cc
/// \brief Implementation of the B3::DetectorConstruction class

#include "DetectorConstruction.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4NistManager.hh"
#include "G4PSDoseDeposit.hh"
#include "G4PSEnergyDeposit.hh"
#include "G4PVPlacement.hh"
#include "G4PhysicalConstants.hh"
#include "G4RotationMatrix.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4Transform3D.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"

namespace B3
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
{
  DefineMaterials();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::DefineMaterials()
{
  G4NistManager* man = G4NistManager::Instance();

  G4bool isotopes = false;

  G4Element* O = man->FindOrBuildElement("O", isotopes);
  G4Element* Si = man->FindOrBuildElement("Si", isotopes);
  G4Element* Lu = man->FindOrBuildElement("Lu", isotopes);

  auto LSO = new G4Material("Lu2SiO5", 7.4 * g / cm3, 3);
  LSO->AddElement(Lu, 2);
  LSO->AddElement(Si, 1);
  LSO->AddElement(O, 5);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Gamma detector Parameters
  //
  G4double cryst_dX = 6 * cm, cryst_dY = 6 * cm, cryst_dZ = 3 * cm;
  G4int nb_cryst = 32;
  G4int nb_rings = 9;
  //
  G4double dPhi = twopi / nb_cryst, half_dPhi = 0.5 * dPhi;
  G4double cosdPhi = std::cos(half_dPhi);
  G4double tandPhi = std::tan(half_dPhi);
  //
  G4double ring_R1 = 0.5 * cryst_dY / tandPhi;
  G4double ring_R2 = (ring_R1 + cryst_dZ) / cosdPhi;
  //
  //G4double detector_dZ = nb_rings * cryst_dX;

  //Materials
  G4NistManager* nist = G4NistManager::Instance();
  G4Material* default_mat = nist->FindOrBuildMaterial("G4_AIR");
  G4Material* cryst_mat = nist->FindOrBuildMaterial("Lu2SiO5");

  //
  // World
  //
  G4double world_sizeXY = 80.69 * cm;
  G4double world_sizeZ = 64.8 * cm;

  auto solidWorld =
    new G4Box("World",  // its name
              0.5 * world_sizeXY, 0.5 * world_sizeXY, 0.5 * world_sizeZ);  // its size

  auto logicWorld = new G4LogicalVolume(solidWorld,  // its solid
                                        default_mat,  // its material
                                        "World");  // its name

  auto physWorld = new G4PVPlacement(nullptr,  // no rotation
                                     G4ThreeVector(),  // at (0,0,0)
                                     logicWorld,  // its logical volume
                                     "World",  // its name
                                     nullptr,  // its mother  volume
                                     false,  // no boolean operation
                                     0,  // copy number
                                     fCheckOverlaps);  // checking overlaps

//
// Tạo khối detector hình hộp chữ nhật
//
  G4double detector_dX = 40 * cm;  // Kích thước theo trục X
  G4double detector_dY = 40 * cm;  // Kích thước theo trục Y
  G4double detector_dZ = 5 * cm;   // Kích thước theo trục Z

  // Tạo hình hộp chữ nhật
  auto solidDetector = new G4Box("Detector", detector_dX / 2, detector_dY / 2, detector_dZ / 2);

  // Tạo logical volume cho detector
  auto logicDetector = new G4LogicalVolume(solidDetector, default_mat, "Detector");

  // Đặt detector vào trong thế giới
  new G4PVPlacement(nullptr,  // Không xoay
      G4ThreeVector(),  // Vị trí (0, 0, 0)
      logicDetector,  // Logical volume của detector
      "Detector",  // Tên của detector
      logicWorld,  // Logical volume của thế giới
      false,  // Không có phép toán boolean
      0,  // Số bản sao
      fCheckOverlaps);  // Kiểm tra sự chồng lấp


  //
  // patient
  //
  G4double patient_radius = 8 * cm;
  G4double patient_dZ = 10 * cm;
  G4Material* patient_mat = nist->FindOrBuildMaterial("G4_BRAIN_ICRP");

  auto solidPatient = new G4Tubs("Patient", 0., patient_radius, 0.5 * patient_dZ, 0., twopi);

  auto logicPatient = new G4LogicalVolume(solidPatient,  // its solid
                                          patient_mat,  // its material
                                          "PatientLV");  // its name

  //
  // place patient in world
  //
  new G4PVPlacement(nullptr,  // no rotation
                    G4ThreeVector(),  // at (0,0,0)
                    logicPatient,  // its logical volume
                    "Patient",  // its name
                    logicWorld,  // its mother  volume
                    false,  // no boolean operation
                    0,  // copy number
                    fCheckOverlaps);  // checking overlaps

  // Visualization attributes
  //
  //logicRing->SetVisAttributes(G4VisAttributes::GetInvisible());
  logicDetector->SetVisAttributes(G4VisAttributes::GetInvisible());

  // Print materials
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;

  // always return the physical World
  //
  return physWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructSDandField()
{
  G4SDManager::GetSDMpointer()->SetVerboseLevel(1);

  // declare crystal as a MultiFunctionalDetector scorer
  //
  // Tạo MultiFunctionalDetector cho detector hình hộp chữ nhật
  auto detector = new G4MultiFunctionalDetector("Detector");
  G4SDManager::GetSDMpointer()->AddNewDetector(detector);

  // Đặt scorer cho detector để ghi nhận năng lượng nạp vào
  G4VPrimitiveScorer* primitiv = new G4PSEnergyDeposit("edep");
  detector->RegisterPrimitive(primitiv);

  // Đặt detector làm sensitive detector
  SetSensitiveDetector("Detector", detector);

  // declare patient as a MultiFunctionalDetector scorer
  //
  auto patient = new G4MultiFunctionalDetector("patient");
  G4SDManager::GetSDMpointer()->AddNewDetector(patient);
  G4VPrimitiveScorer* primitiv2 = new G4PSDoseDeposit("dose");
  patient->RegisterPrimitive(primitiv2);
  SetSensitiveDetector("PatientLV", patient);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}  // namespace B3
