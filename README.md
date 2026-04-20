# Projet-BTSCIEL-TRAPPLER-Ethann

# 💧 Débitmètre Solaire – BTS CIEL

## 📌 Présentation

Ce projet a pour objectif de **mesurer la consommation d’eau dans des zones isolées** en utilisant un système autonome alimenté par énergie solaire.

Le système repose sur une architecture en trois parties :
- 🟦 Carte électronique (ER) → mesure eau + énergie
- 🟩 Raspberry Pi (IR – passerelle) → traitement + affichage + transmission
- 🟨 Serveur Web (IR2) → stockage + visualisation

---

## 🎯 Problématique

> Comment mesurer une consommation d’eau dans des endroits isolés avec une faible consommation énergétique ?

---

## 🧠 Architecture globale


[ Carte ER ]
│ (UART / Série)
▼
[ Raspberry Pi ]
│ (HTTP / 3G / Réseau)
▼
[ Serveur Web + BDD ]


---

## ⚙️ Fonctionnement

### 🔁 Cycle complet

1. La carte ER alimente le Raspberry
2. Le Raspberry démarre automatiquement (systemd)
3. Envoi de l’heure à la carte (`TIME`)
4. Demande des données (`REQUEST`)
5. Réception des données (`WATER=...;ENERGY=...`)
6. Traitement + horodatage
7. Affichage sur écran OLED (I2C)
8. Envoi vers serveur (HTTP POST)
9. Envoi d’un accusé (`OK`)
10. La carte coupe l’alimentation (économie d’énergie)

---

## 🔌 Communication série (UART)

### 📡 Configuration
- Baudrate : 9600
- Format : 8N1
- Protocole texte
- Fin de trame : `\n`

### 📨 Protocole


Raspberry → ER
TIME:YYYY-MM-DD HH:MM:SS\n
REQUEST\n

ER → Raspberry
WATER=XXXX;ENERGY=XX.X\n

Raspberry → ER
OK\n


---

## 🌐 Transmission réseau

### 📤 Format HTTP POST


water=1234&energy=45.6&date=2026-03-31&time=10:30:00


### 🔧 Technologies utilisées
- libcurl (C++)
- PHP (API serveur)
- MySQL (base de données)

---

## 🖥️ Affichage OLED (I2C)

- Écran OLED 128x64
- Communication I2C (adresse `0x3C`)
- Pilotage bas niveau (sans bibliothèque externe)
- Police personnalisée 5x7
- Gestion directe des pixels via buffer

---

## 🧱 Structure du projet


projet_raspberry/
│
├── bin/ → exécutable
├── logs/ → logs système
│
├── src/
│ ├── display/ → écran OLED (I2C)
│ ├── er/ → parsing + protocole
│ ├── network/ → HTTP (libcurl)
│ ├── serial/ → communication UART
│ ├── simulateur/ → simulation ER
│ ├── data/ → structure Mesure
│ └── utils/ → temps + logs
│
└── main.cpp → logique principale


---

## 🚀 Lancement automatique

Le programme démarre automatiquement au boot via **systemd** :


/etc/systemd/system/er_system.service


Fonctionnalités :
- démarrage automatique
- gestion du proxy réseau
- redémarrage en cas d’erreur
- logs centralisés

---

## 📊 État actuel

✔ Communication série fonctionnelle  
✔ Protocole TIME / REQUEST / OK validé  
✔ Parsing et validation des données  
✔ Affichage OLED opérationnel  
✔ Envoi HTTP vers serveur fonctionnel  
✔ Lancement automatique au démarrage  

---

## ⚠️ Limites actuelles

- Pas de gestion avancée des erreurs série
- Pas de stockage local en cas de perte réseau
- Pas de checksum sur les trames
- Communication HTTP non sécurisée (HTTP)

---

## 🔧 Améliorations possibles

- Ajout de timeout série + retry
- Buffer local en cas de perte réseau
- Passage à HTTPS
- Ajout d’un CRC sur les trames
- Utilisation d’un protocole IoT (MQTT)
- Optimisation énergétique avancée

---

## 🧑‍💻 Auteur

Projet réalisé dans le cadre du BTS CIEL  
Spécialité Informatique & Réseaux

---

## 📜 Licence

Projet pédagogique – usage académique
