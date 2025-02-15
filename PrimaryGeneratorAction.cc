﻿//
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
/// \file B3/B3a/src/PrimaryGeneratorAction.cc
/// \brief Implementation of the B3::PrimaryGeneratorAction class

#include "PrimaryGeneratorAction.hh"

#include "G4ChargedGeantino.hh"
#include "G4IonTable.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

namespace B3
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction()
{
  G4int n_particle = 50;
  fParticleGun = new G4ParticleGun(n_particle);

  // default particle kinematic

  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particle = particleTable->FindParticle("gamma");
  fParticleGun->SetParticleDefinition(particle);
  fParticleGun->SetParticlePosition(G4ThreeVector(0., 0., -31.));
  fParticleGun->SetParticleEnergy(662 * keV);
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0, 0, 1.));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
	// Vị trí phát ngẫu nhiên (tuỳ chỉnh vùng không gian nhỏ xung quanh vị trí gốc)
	G4double x0 = 0 * cm, y0 = 0 * cm, z0 = -31 * cm;
	G4double dx0 = 0.1 * cm, dy0 = 0.1 * cm, dz0 = 0.1 * cm;
	x0 += dx0 * (G4UniformRand() - 0.1);
	y0 += dy0 * (G4UniformRand() - 0.1);
	z0 += dz0 * (G4UniformRand() - 0.1);
	fParticleGun->SetParticlePosition(G4ThreeVector(x0, y0, z0));

	// Tạo hướng động lượng ngẫu nhiên trong hình nón
	G4double coneAngle = 9 * deg;  // Góc hình nón
	G4double cosTheta = std::cos(coneAngle);
	G4double z = cosTheta + (1 - cosTheta) * G4UniformRand();
	G4double phi = 2 * CLHEP::pi * G4UniformRand();
	G4double sinTheta = std::sqrt(1 - z * z);

	G4double px = sinTheta * std::cos(phi);
	G4double py = sinTheta * std::sin(phi);
	G4double pz = z;

	fParticleGun->SetParticleMomentumDirection(G4ThreeVector(px, py, pz));

	// Phát hạt gamma
	fParticleGun->GeneratePrimaryVertex(event);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}  // namespace B3
