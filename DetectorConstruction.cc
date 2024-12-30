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

  // Khai báo các nguyên tố
  G4Element* Cd = new G4Element("Cadmium", "Cd", 1., 112.41 * g / mole);
  G4Element* Zn = new G4Element("Zinc", "Zn", 1., 65.38 * g / mole);
  G4Element* Te = new G4Element("Tellurium", "Te", 1., 127.60 * g / mole);

  // Khai báo vật liệu CZT với tỷ lệ Zn = 0.2 (20%)
  auto CZT = new G4Material("CZT", 5.85 * g / cm3, 3);
  CZT->AddElement(Cd, 0.5);
  CZT->AddElement(Zn, 0.2);
  CZT->AddElement(Te, 0.3);

  //Khai bao YSO(Ce)
  G4Element* Y = man->FindOrBuildElement("Y");
  //G4Element* Si = man->FindOrBuildElement("Si");
  //G4Element* O = man->FindOrBuildElement("O");
  G4Element* Ce = man->FindOrBuildElement("Ce");

  // Mat do vat lieu
  G4double density = 4.5 * g / cm3;

  // Đinh nghia vat lieu Y2SiO5:Ce voi ty le phan tram khoi luong
  G4Material* Y2SiO5_Ce = new G4Material("Y2SiO5_Ce", density, 4);

  // Tỷ lệ phần trăm khối lượng (giả sử Ce là 0.1%)
  G4double massFraction_Y = 0.635;   // 63.5% Y
  G4double massFraction_Si = 0.155; // 15.5% Si
  G4double massFraction_O = 0.209;  // 20.9% O
  G4double massFraction_Ce = 0.001; // 0.1% Ce

  // Thêm các nguyên tố theo tỷ lệ khối lượng
  Y2SiO5_Ce->AddElement(Y, massFraction_Y);
  Y2SiO5_Ce->AddElement(Si, massFraction_Si);
  Y2SiO5_Ce->AddElement(O, massFraction_O);
  Y2SiO5_Ce->AddElement(Ce, massFraction_Ce);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Gamma detector Parameters
  //
  G4bool fCheckOverlaps = true;
  //G4double detector_dZ = nb_rings * cryst_dX;

  //Materials
  G4NistManager* nist = G4NistManager::Instance();
  G4Material* default_mat = nist->FindOrBuildMaterial("G4_AIR");
  G4Material* cryst_mat = nist->FindOrBuildMaterial("Y2SiO5_Ce");

  //
  // World
  //
  G4double world_sizeXY = 20 * cm;
  G4double world_sizeZ = 50 * cm;

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
// Khoi hap thu cryst_mat
//
  G4double detector_dX = 10 * cm;  // Kích thước theo trục X
  G4double detector_dY = 10 * cm;  // Kích thước theo trục Y
  G4double detector_dZ = 10 * cm;   // Kích thước theo trục Z

  // Tạo hình hộp chữ nhật
  auto solidDetector = new G4Box("Detector", detector_dX / 2, detector_dY / 2, detector_dZ / 2);

  // Tạo logical volume cho detector
  auto logicDetector = new G4LogicalVolume(solidDetector, cryst_mat, "DetectorLV");

  // Đặt detector vào trong thế giới
  new G4PVPlacement(nullptr,  // Không xoay
      G4ThreeVector(0, 0, 10*cm),  // Vị trí (0, 0, 0)
      logicDetector,  // Logical volume của detector
      "Detector",  // Tên của detector
      logicWorld,  // Logical volume của thế giới
      false,  // Không có phép toán boolean
      0,  // Số bản sao
      fCheckOverlaps);  // Kiểm tra sự chồng lấp


  //
  // Khoi tan xa cryst_mat
  //
  G4double compSi_X = 10 * cm;
  G4double compSi_Y = 10 * cm;
  //G4Material* compSi_mat = nist->FindOrBuildMaterial("G4_Si");

  auto solidCompSi = new G4Box("Patient", compSi_X / 2, compSi_Y / 2, compSi_Y / 10);
  //auto solidPatient = new G4Tubs("Patient", 0., patient_radius, 0.5 * patient_dZ, 0., twopi);

  auto logicCompSi = new G4LogicalVolume(solidCompSi,  // its solid
                                          cryst_mat,  // its material
                                          "PatientLV");  // its name

  //
  // place patient in world
  //
  new G4PVPlacement(nullptr,  // no rotation
                    G4ThreeVector(),  // at (0,0,0)
                    logicCompSi,  // its logical volume
                    "Patient",  // its name
                    logicWorld,  // its mother  volume
                    false,  // no boolean operation
                    0,  // copy number
                    fCheckOverlaps);  // checking overlaps

  // Visualization attributes
  //
  //logicRing->SetVisAttributes(G4VisAttributes::GetInvisible());
  //logicDetector->SetVisAttributes(G4VisAttributes::GetInvisible());

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
  auto detector = new G4MultiFunctionalDetector("detector");
  G4SDManager::GetSDMpointer()->AddNewDetector(detector);
  G4VPrimitiveScorer* primitiv = new G4PSEnergyDeposit("edep");
  detector->RegisterPrimitive(primitiv);
  SetSensitiveDetector("DetectorLV", detector);

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
