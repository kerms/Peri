-- MySQL Workbench Forward Engineering

SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='TRADITIONAL,ALLOW_INVALID_DATES';

-- -----------------------------------------------------
-- Schema zoo
-- -----------------------------------------------------
DROP SCHEMA IF EXISTS `zoo` ;

-- -----------------------------------------------------
-- Schema zoo
-- -----------------------------------------------------
CREATE SCHEMA IF NOT EXISTS `zoo` DEFAULT CHARACTER SET utf8 ;
USE `zoo` ;

-- -----------------------------------------------------
-- Table `zoo`.`enclos`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `zoo`.`enclos` ;

CREATE TABLE IF NOT EXISTS `zoo`.`enclos` (
  `idenclos` INT NOT NULL AUTO_INCREMENT,
  `nom` VARCHAR(45) NOT NULL,
  `superficie` INT NOT NULL,
  `date_de_construction` DATETIME NOT NULL,
  `description` VARCHAR(45) NOT NULL,
  PRIMARY KEY (`idenclos`),
  UNIQUE INDEX `idenclos_UNIQUE` (`idenclos` ASC))
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `zoo`.`type_capteur`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `zoo`.`type_capteur` ;

CREATE TABLE IF NOT EXISTS `zoo`.`type_capteur` (
  `idtype_capteur` INT NOT NULL AUTO_INCREMENT,
  `nom` VARCHAR(45) NOT NULL,
  PRIMARY KEY (`idtype_capteur`),
  UNIQUE INDEX `idtype_UNIQUE` (`idtype_capteur` ASC))
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `zoo`.`capteur`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `zoo`.`capteur` ;

CREATE TABLE IF NOT EXISTS `zoo`.`capteur` (
  `idcapteur` INT NOT NULL AUTO_INCREMENT,
  `nom` VARCHAR(45) NOT NULL,
  `description` VARCHAR(45) NOT NULL,
  `enclos_idenclos` INT NOT NULL,
  `type_capteur_idtype_capteur` INT NOT NULL,
  PRIMARY KEY (`idcapteur`),
  UNIQUE INDEX `idcapteur_UNIQUE` (`idcapteur` ASC),
  INDEX `fk_capteur_enclos_idx` (`enclos_idenclos` ASC),
  INDEX `fk_capteur_type_capteur1_idx` (`type_capteur_idtype_capteur` ASC),
  CONSTRAINT `fk_capteur_enclos`
    FOREIGN KEY (`enclos_idenclos`)
    REFERENCES `zoo`.`enclos` (`idenclos`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_capteur_type_capteur1`
    FOREIGN KEY (`type_capteur_idtype_capteur`)
    REFERENCES `zoo`.`type_capteur` (`idtype_capteur`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `zoo`.`mesure`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `zoo`.`mesure` ;

CREATE TABLE IF NOT EXISTS `zoo`.`mesure` (
  `idmesure` INT NOT NULL AUTO_INCREMENT,
  `valeur` FLOAT NOT NULL,
  `date` DATETIME NOT NULL,
  `capteur_idcapteur` INT NOT NULL,
  PRIMARY KEY (`idmesure`),
  UNIQUE INDEX `idmesure_UNIQUE` (`idmesure` ASC),
  INDEX `fk_mesure_capteur1_idx` (`capteur_idcapteur` ASC),
  CONSTRAINT `fk_mesure_capteur1`
    FOREIGN KEY (`capteur_idcapteur`)
    REFERENCES `zoo`.`capteur` (`idcapteur`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `zoo`.`employe`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `zoo`.`employe` ;

CREATE TABLE IF NOT EXISTS `zoo`.`employe` (
  `idemploye` INT NOT NULL AUTO_INCREMENT,
  `nom` VARCHAR(45) NOT NULL,
  `prenom` VARCHAR(45) NOT NULL,
  PRIMARY KEY (`idemploye`),
  UNIQUE INDEX `idemploye_UNIQUE` (`idemploye` ASC))
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `zoo`.`reparateur`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `zoo`.`reparateur` ;

CREATE TABLE IF NOT EXISTS `zoo`.`reparateur` (
  `idreparateur` INT NOT NULL AUTO_INCREMENT,
  `nom` VARCHAR(45) NOT NULL,
  `prenom` VARCHAR(45) NOT NULL,
  PRIMARY KEY (`idreparateur`),
  UNIQUE INDEX `idreparateur_UNIQUE` (`idreparateur` ASC))
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `zoo`.`enclos_has_employe`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `zoo`.`enclos_has_employe` ;

CREATE TABLE IF NOT EXISTS `zoo`.`enclos_has_employe` (
  `enclos_idenclos` INT NOT NULL,
  `employe_idemploye` INT NOT NULL,
  PRIMARY KEY (`enclos_idenclos`, `employe_idemploye`),
  INDEX `fk_enclos_has_employe_employe1_idx` (`employe_idemploye` ASC),
  INDEX `fk_enclos_has_employe_enclos1_idx` (`enclos_idenclos` ASC),
  CONSTRAINT `fk_enclos_has_employe_enclos1`
    FOREIGN KEY (`enclos_idenclos`)
    REFERENCES `zoo`.`enclos` (`idenclos`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_enclos_has_employe_employe1`
    FOREIGN KEY (`employe_idemploye`)
    REFERENCES `zoo`.`employe` (`idemploye`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `zoo`.`capteur_has_reparateur`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `zoo`.`capteur_has_reparateur` ;

CREATE TABLE IF NOT EXISTS `zoo`.`capteur_has_reparateur` (
  `capteur_idcapteur` INT NOT NULL,
  `reparateur_idreparateur` INT NOT NULL,
  PRIMARY KEY (`capteur_idcapteur`, `reparateur_idreparateur`),
  INDEX `fk_capteur_has_reparateur_reparateur1_idx` (`reparateur_idreparateur` ASC),
  INDEX `fk_capteur_has_reparateur_capteur1_idx` (`capteur_idcapteur` ASC),
  CONSTRAINT `fk_capteur_has_reparateur_capteur1`
    FOREIGN KEY (`capteur_idcapteur`)
    REFERENCES `zoo`.`capteur` (`idcapteur`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_capteur_has_reparateur_reparateur1`
    FOREIGN KEY (`reparateur_idreparateur`)
    REFERENCES `zoo`.`reparateur` (`idreparateur`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `zoo`.`type_animal`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `zoo`.`type_animal` ;

CREATE TABLE IF NOT EXISTS `zoo`.`type_animal` (
  `idtype_animal` INT NOT NULL AUTO_INCREMENT,
  `nom` VARCHAR(45) NOT NULL,
  PRIMARY KEY (`idtype_animal`),
  UNIQUE INDEX `idtype_animal_UNIQUE` (`idtype_animal` ASC))
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `zoo`.`animal`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `zoo`.`animal` ;

CREATE TABLE IF NOT EXISTS `zoo`.`animal` (
  `idanimal` INT NOT NULL AUTO_INCREMENT,
  `petitnom` VARCHAR(45) NULL,
  `date_naissance` DATETIME NOT NULL,
  `enclos_idenclos` INT NOT NULL,
  `type_animal_idtype_animal` INT NOT NULL,
  PRIMARY KEY (`idanimal`),
  UNIQUE INDEX `idanimal_UNIQUE` (`idanimal` ASC),
  INDEX `fk_animal_enclos1_idx` (`enclos_idenclos` ASC),
  INDEX `fk_animal_type_animal1_idx` (`type_animal_idtype_animal` ASC),
  CONSTRAINT `fk_animal_enclos1`
    FOREIGN KEY (`enclos_idenclos`)
    REFERENCES `zoo`.`enclos` (`idenclos`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_animal_type_animal1`
    FOREIGN KEY (`type_animal_idtype_animal`)
    REFERENCES `zoo`.`type_animal` (`idtype_animal`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;
