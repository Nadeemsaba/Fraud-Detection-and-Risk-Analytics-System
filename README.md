# Fraud-Detection-and-Risk-Analytics-System

A full-stack web application designed to detect suspicious financial
transactions using rule-based risk evaluation and dynamic trust score
management.

------------------------------------------------------------------------

## 📌 Project Overview

The Fraud Detection and Risk Analytics System monitors user transactions
in real-time, evaluates fraud risk based on predefined rules, and
dynamically updates user trust scores.

### Key Features:

-   Secure JWT Authentication
-   Rule-Based Fraud Detection Engine
-   Dynamic Trust Score Management
-   Admin Monitoring Dashboard
-   Risk Analytics Visualization
-   Device & Country Tracking

------------------------------------------------------------------------

## 🏗️ System Architecture

The system follows a three-tier architecture:

1.  Presentation Layer -- React.js (Frontend)\
2.  Application Layer -- C++ (Crow Framework)\
3.  Data Layer -- SQLite Database

Flow:\
User → Frontend → Backend → Fraud Engine → Database → Dashboard

------------------------------------------------------------------------

## 🛠️ Technology Stack

### Backend

-   C++
-   Crow Web Framework
-   SQLite
-   OpenSSL (SHA-256 Hashing)
-   JWT Authentication

### Frontend

-   React.js
-   Axios
-   React Router

### Database

-   SQLite

------------------------------------------------------------------------

## 🔐 Authentication & Security

-   SHA-256 password hashing
-   JWT-based authentication
-   Role-Based Access Control (USER / ADMIN)
-   Block/Unblock user mechanism
-   Device tracking
-   Server-side validation

------------------------------------------------------------------------

## 📊 Fraud Detection Logic

Risk is evaluated based on:

-   Transaction Amount
-   Country Mismatch
-   New Device Detection
-   Transaction Frequency
-   Spending Behavior Analysis

### Risk Classification

  Risk Score   Level
  ------------ --------
  0 -- 39      LOW
  40 -- 59     MEDIUM
  60 -- 79     HIGH
  80+          FRAUD

------------------------------------------------------------------------

## 📈 Trust Score Mechanism

-   Risk ≥ 80 → -20\
-   Risk ≥ 60 → -10\
-   Risk ≥ 40 → -5\
-   Safe transaction → +3

Trust Score Range: 0 -- 100

------------------------------------------------------------------------

## 👤 User Features

-   Register & Login
-   Create Transaction
-   View Trust Score
-   View Last 20 Transactions
-   View Risk Level

------------------------------------------------------------------------

## 🛡️ Admin Features

-   View All Users
-   Monitor Trust Scores
-   Block/Unblock Users
-   View Risk Analytics Chart

------------------------------------------------------------------------

## ⚙️ Installation & Setup

### Backend

``` bash
cmake .
make
./server
```

Runs at: http://localhost:18080

### Frontend

``` bash
cd frontend
npm install
npm start
```

Runs at: http://localhost:3000

------------------------------------------------------------------------

## 🚀 Future Enhancements

-   Machine Learning-based fraud detection
-   Cloud deployment
-   Real-time IP geolocation API
-   Email/SMS fraud alerts
-   Payment gateway integration
-   Mobile application

------------------------------------------------------------------------

## 📌 License

This project is developed for academic purposes.
