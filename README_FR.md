# Osiris ESP32-S3 — Contrôleur HID Multi-Transports & Compagnon Matériel

[![English Version](https://img.shields.io/badge/Language-English-blue.svg)](README.md)
[![Version Française](https://img.shields.io/badge/Langue-Français-green.svg)](README_FR.md)

Osiris est un contrôleur d'interface humaine (HID) open-source multi-transports (USB natif OTG et Bluetooth Low Energy), simulant un clavier, une souris indétectable ("mouse jiggler"), et un tableau de bord de télémétrie matériel alimenté par l'**ESP32-S3** (N16R8).

Il associe une interface physique locale (écran OLED 0.96" avec menu de navigation à 2 boutons) à une application Web complète hébergée directement dans la mémoire flash de l'ESP32-S3, accessible via son Point d'Accès WiFi intégré ou votre réseau local.

---

## Fonctionnalités Principales

* **Émulation HID Double Transport** :
  * **USB OTG Natif** : Émulation matérielle de clavier et souris USB via le contrôleur USB PHY interne de l'ESP32-S3 (TinyUSB).
  * **Bluetooth Low Energy (BLE)** : Clavier sans fil pour smartphones, tablettes et ordinateurs.
  * **Arbitrage Intelligent des Transports** : Désactive automatiquement le Bluetooth dès qu'une connexion USB physique est détectée sur l'ordinateur cible ("USB ou BLE, jamais les deux en même temps").
* **Mouse Jiggler Indétectable** :
  * Micro-déplacements réguliers toutes les 2 secondes avec durées configurables (30 min, 60 min, 90 min, ou illimité).
  * Architecture entièrement non bloquante par machine d'état (aucun ralentissement de l'interface ni perte de clics).
* **Interface Physique & Écran OLED** :
  * Écran OLED monochrome I2C 0.96" (SSD1306, 128x64) cadencé à 400 kHz (mode rapide).
  * Navigation gestuelle à 2 boutons (`OneButton` : simple appui, double appui, appui long).
  * Affichage en mode Liste ou Grille avec défilement fluide.
  * Prise en charge multilingue native : Français, Anglais et Allemand.
  * Veille matérielle de l'écran (`SSD1306_DISPLAYOFF`, consommation < 10 µA) avec réveil instantané sur action.
* **Portail Web Embarqué & API WebSocket** :
  * Interface servie depuis la mémoire interne (`index_html.h`) via `ESPAsyncWebServer`.
  * Communication bidirectionnelle en temps réel via WebSocket (`/ws`).
  * Tableau de bord en direct : heure/date du PC, température interne du processeur ESP32, état du réseau et nombre de clients connectés.
  * **Clavier Virtuel à Distance** : Saisie de texte en direct et injection de macros (`CTRL+ALT+DEL`, `WIN+R`, `ALT+TAB`, touches de fonction F1-F12).
  * **Coffre-fort de Mots de Passe (NVS)** : Mémorisation persistante de 8 identifiants en mémoire flash ; injection automatique du nom d'utilisateur, touche Tabulation, et mot de passe en un seul clic.
  * **Scanner WiFi & Mode Station** : Détection des réseaux 2.4 GHz environnants (force du signal RSSI, chiffrement) et connexion au réseau local.
  * **Résolution mDNS** : Accès direct via `http://osiris-device.local`.

---

## Schéma Matériel & Brochage

Pour le schéma complet des liaisons, les diagrammes ASCII et les détails d'intégration, consultez [CABLAGE.md](CABLAGE.md).

![Brochage Osiris ESP32-S3](ESP32-S3%20Pinouts.png)

| Composant | Broche Composant | Broche ESP32-S3 | Fonction |
| :--- | :--- | :--- | :--- |
| **Écran OLED 0.96" (SSD1306)** | `SDA` | **GPIO 8** | Données I2C (Mode Rapide 400 kHz) |
| **Écran OLED 0.96" (SSD1306)** | `SCL` | **GPIO 9** | Horloge I2C (Mode Rapide 400 kHz) |
| **Écran OLED 0.96" (SSD1306)** | `VCC` | **3.3V** | Alimentation 3.3V |
| **Écran OLED 0.96" (SSD1306)** | `GND` | **GND** | Masse commune |
| **Bouton Haut (`BTN_TOP`)** | Broche 1 -> GPIO, Broche 2 -> GND | **GPIO 4** | Actif à l'état BAS, Pull-Up interne |
| **Bouton Bas (`BTN_BOTTOM`)** | Broche 1 -> GPIO, Broche 2 -> GND | **GPIO 5** | Actif à l'état BAS, Pull-Up interne |
| **PC Cible (HID)** | Port USB Type-C Natif | **GPIO 19 (D-) / GPIO 20 (D+)** | USB OTG Natif (TinyUSB) |
| **PC Hôte (Série/CMD)** | Port USB Type-C UART | **GPIO 43 (TX) / GPIO 44 (RX)** | Pont CH343 / CH340 (115200 bauds) |

---

## Structure du Dépôt

```
Osiris-ESP32S3/
├── firmware/
│   └── Osiris/
│       ├── Osiris.ino          # Programme principal & machine d'état
│       ├── ble_keys.cpp        # Gestion du clavier Bluetooth LE HID
│       ├── ble_keys.h
│       ├── usb_keys.cpp        # Gestion du clavier/souris USB OTG TinyUSB
│       ├── usb_keys.h
│       ├── net_scan.cpp        # Scanner WiFi
│       ├── net_scan.h
│       ├── index_html.h        # Interface Web embarquée (HTML/CSS/JS)
│       ├── partitions.csv      # Table des partitions Flash 16 Mo
│       ├── setup.txt           # Configuration Arduino IDE (Anglais)
│       └── setup_FR.txt        # Configuration Arduino IDE (Français)
├── pics/                       # Photos du montage sur carte perforée
│   ├── README.md
│   ├── README_FR.md
│   └── IMG_20260808_*.jpg      # Vues avant OLED et soudures arrière
├── docs/
│   ├── WIRING.md               # Guide de câblage détaillé (Anglais)
│   └── CABLAGE.md              # Guide de câblage détaillé (Français)
├── ESP32-S3 Pinouts.png        # Schéma annoté du brochage ESP32-S3
├── WIRING.md                   # Accès rapide câblage (Anglais)
├── CABLAGE.md                  # Accès rapide câblage (Français)
├── README.md                   # Documentation principale (Anglais)
├── README_FR.md                # Documentation principale (Français)
├── LICENSE                     # Licence MIT
└── .gitignore                  # Fichiers ignorés par Git
```

---

## Guide de Compilation et Flashage

### 1. Préparation de l'IDE Arduino
1. Installez **Arduino IDE** (version 2.x recommandée).
2. Ajoutez le gestionnaire de cartes ESP32 dans les préférences de l'IDE :
   * URL : `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
   * Installez le paquet `esp32` par Espressif Systems (version 2.0.14 ou ultérieure).

### 2. Bibliothèques Requises
Installez les bibliothèques suivantes depuis le Gestionnaire de Bibliothèques (`Ctrl+Maj+I`) :
* `OneButton` par Matthias Hertel
* `Adafruit SSD1306` & `Adafruit GFX Library`
* `AsyncTCP` et `ESPAsyncWebServer`
* `ESP32-BLE-Keyboard` par T-vK (disponible sur GitHub)

### 3. Paramètres de la Carte dans l'IDE Arduino
Ouvrez `firmware/Osiris/Osiris.ino` et configurez le menu **Outils** :

| Paramètre | Valeur Requise | Explication |
| :--- | :--- | :--- |
| **Carte (Board)** | `ESP32S3 Dev Module` | Module ESP32-S3 standard |
| **Flash Size** | `16MB (128Mb)` | Correspond à la mémoire flash N16 |
| **Partition Scheme** | `16M Flash (3MB APP/9.9MB FATFS)` *(ou `Huge APP (3MB No OTA/1MB SPIFFS)`)* | Porte la limite de compilation de 1,25 Mo à 3 Mo ; `firmware/Osiris/partitions.csv` fournit le partitionnement 16 Mo personnalisé |
| **PSRAM** | `OPI PSRAM` | Correspond à la mémoire PSRAM octale R8 |
| **USB Mode** | `USB-OTG (TinyUSB)` | **Indispensable** : Active l'émulation HID |
| **USB CDC On Boot** | `Disabled` | **Indispensable** : Conserve la liaison Série sur le port CH343 |
| **Upload Speed** | `921600` | Vitesse de téléversement rapide recommandée |

---

## Identifiants et Accès par Défaut

Le micrologiciel est préconfiguré avec des identifiants génériques prêts à l'emploi :

* **Nom du Point d'Accès (SSID)** : `Osiris_AP`
* **Mot de passe du Point d'Accès** : `Password1234`
* **Adresse Web locale** : `http://192.168.4.1` (ou `http://osiris-device.local` via mDNS)
* **Vitesse du port Série** : `115200 bauds` (sur le second port USB UART)

Vous pouvez modifier ces identifiants à tout moment directement dans `firmware/Osiris/Osiris.ino` ou dynamiquement via le portail Web.

---

## Licence

Ce projet est distribué sous licence open-source [Licence MIT](LICENSE).
