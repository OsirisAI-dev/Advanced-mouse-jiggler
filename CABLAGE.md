# Osiris ESP32-S3 — Schéma de Câblage Matériel et Brochage

[![English Version](https://img.shields.io/badge/Language-English-blue.svg)](WIRING.md)
[![Version Française](https://img.shields.io/badge/Langue-Français-green.svg)](CABLAGE.md)

Ce document décrit en détail l'affectation physique des broches et le schéma de câblage du système Osiris basé sur l'ESP32-S3.

---

## 1. Tableau Récapitulatif du Brochage

| Composant | Broche Composant | Broche ESP32-S3 | Mode Matériel | Fonction |
| :--- | :--- | :--- | :--- | :--- |
| **Écran OLED 0.96" (SSD1306)** | `SDA` | **GPIO 8** | I2C Matériel | Ligne de données I2C (Mode Rapide 400 kHz) |
| **Écran OLED 0.96" (SSD1306)** | `SCL` | **GPIO 9** | I2C Matériel | Ligne d'horloge I2C (Mode Rapide 400 kHz) |
| **Écran OLED 0.96" (SSD1306)** | `VCC` | **3V3** | Alimentation | Rail d'alimentation 3.3V |
| **Écran OLED 0.96" (SSD1306)** | `GND` | **GND** | Masse commune | Masse générale |
| **Bouton Haut (`BTN_TOP`)** | Borne A | **GPIO 4** | `INPUT_PULLUP` | Résistance de tirage interne activée |
| | Borne B | **GND** | Masse commune | Actif à l'état BAS lors de l'appui |
| **Bouton Bas (`BTN_BOTTOM`)** | Borne A | **GPIO 5** | `INPUT_PULLUP` | Résistance de tirage interne activée |
| | Borne B | **GND** | Masse commune | Actif à l'état BAS lors de l'appui |
| **PC Cible (HID)** | `USB D- (DM)` | **GPIO 19** | USB OTG Natif | Port USB-C 1 (Clavier & Souris HID) |
| **PC Cible (HID)** | `USB D+ (DP)` | **GPIO 20** | USB OTG Natif | Port USB-C 1 (Clavier & Souris HID) |
| **PC Hôte (Série/CMD)** | `UART TX` | **GPIO 43** | Serial0 TX | Routé vers la puce CH343 (115200 bauds) |
| **PC Hôte (Série/CMD)** | `UART RX` | **GPIO 44** | Serial0 RX | Routé vers la puce CH343 (115200 bauds) |

---

## 2. Schéma de Câblage Détaillé (ASCII)

```
                       +-----------------------------------+
                       |      ESP32-S3-WROOM-1 / N16R8     |
                       |                                   |
    [ PC CIBLE ] <====>| [Port USB-C 1: OTG Natif]         |
     (Clavier HID /    |   (GPIO 19: D-, GPIO 20: D+)      |
      Souris Jiggler)  |                                   |
                       |                                   |
    [ PC HÔTE / CMD ]<=| [Port USB-C 2: Pont UART CH343]   |
     (Série 115200 /   |   (GPIO 43: TX, GPIO 44: RX)      |
      Flash/Débogage)  |                                   |
                       |                                   |
                       |       3.3V o-----------------------------o VCC [Écran OLED]
                       |     GPIO 9 o-----------------------------o SCL [128x64 SSD1306]
                       |     GPIO 8 o-----------------------------o SDA (I2C 400 kHz)
                       |        GND o---+-------------------------o GND
                       |                |
                       |     GPIO 4 o---)---[ BTN_TOP ]---+    (Menu Haut / Bascule Jiggler)
                       |                |                 |
                       |     GPIO 5 o---)---[ BTN_BOTTOM ]+    (Menu Bas / Retour)
                       |                |                 |
                       |                +-----------------+---o GND
                       +-----------------------------------+
```

---

## 3. Montage Physique sur Carte Perforée

D'après le schéma de brochage matériel (`ESP32-S3 Pinouts.png`) et les photos du dossier `pics/` :

1. **Module Contrôleur (ESP32-S3-N16R8 DevKitC)** :
   - Les deux ports USB Type-C fournissent respectivement l'USB-OTG natif (`GPIO 19` / `GPIO 20`) pour l'émulation clavier/souris HID vers le PC cible, et l'UART0 matériel (`GPIO 43` / `GPIO 44` via CH343) pour les commandes série et le flashage.
2. **Assemblage sur Plaque d'Essai (`pics/IMG_20260808_*.jpg`)** :
   - Le module ESP32-S3-N16R8 est soudé sur une carte perforée reliant directement le bus I2C et les boutons poussoirs aux broches GPIO.
   - L'écran OLED 0.96" SSD1306 (`SDA` sur `GPIO 8`, `SCL` sur `GPIO 9`) affiche la télémétrie en direct (heure/date du système, température interne de l'ESP32, état WiFi/AP et minuterie du jiggler).
   - Les deux boutons poussoirs tactiles (`BTN_TOP` sur `GPIO 4` et `BTN_BOTTOM` sur `GPIO 5`, résistances `INPUT_PULLUP` activées) pilotent la navigation locale tandis que les frappes HID sont transmises en **USB OTG Natif** ou en **Bluetooth LE**.

---

## 4. Fonction des Boutons Physiques

### Bouton Haut (`BTN_TOP` / GPIO 4)
* **Simple Appui** :
  * Dans le Menu Principal : Déplace le curseur vers le haut (`menuIndex--`)
  * Dans un Sous-Menu : Déplace le curseur vers le haut (`subMenuIndex--`)
* **Double Appui** :
  * Dans le Menu Principal : Entre dans le sous-menu ou exécute l'action directe
  * Dans un Sous-Menu : Valide et applique l'option choisie
* **Appui Long** :
  * Sur l'Écran Principal : Active ou désactive le Mouse Jiggler (active également le Point d'Accès WiFi si besoin).

### Bouton Bas (`BTN_BOTTOM` / GPIO 5)
* **Simple Appui** :
  * Dans le Menu Principal : Déplace le curseur vers le bas (`menuIndex++`)
  * Dans un Sous-Menu : Déplace le curseur vers le bas (`subMenuIndex++`)
* **Double Appui** :
  * Dans un Sous-Menu : Retourne au Menu Principal
  * Dans le Menu Principal : Retourne à l'Écran d'accueil
* **Appui Long** :
  * Sur l'Écran Principal : Ouvre le Menu Principal
