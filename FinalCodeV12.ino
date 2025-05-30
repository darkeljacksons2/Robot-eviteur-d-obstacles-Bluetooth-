#include <Servo.h>
#include <NewPing.h>

// Déclarations des broches
#define SERVO_PIN 10
#define TRIG_PIN 11
#define ECHO_PIN 12

// Broches moteurs
#define in1 3
#define in2 5
#define in3 6
#define in4 9

// Constantes
#define MAX_DISTANCE 150
#define DISTANCE_TO_CHECK 25 // Seuil de distance pour détecter un obstacle
#define REVERSE_DURATION 200 // Durée du recul en millisecondes


Servo monServo;
NewPing mySensor(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);

// Variables pour le mode Bluetooth et automatique
bool modeBluetooth = false; // Par défaut, mode automatique
bool arret = false;         // Arrêt manuel
const int TURN_DELAY = 250  ;     // Durée pour tourner en millisecondes

void setup()
{
  // Configuration des broches
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  // Initialisation des broches à LOW pour éviter une activation involontaire
  Stop();

  // Initialisation du servo
  monServo.attach(SERVO_PIN);
  monServo.write(90); // Position initiale au centre

  // Configuration des communications série
  Serial.begin(9600); // Connexion avec le HC-06 et le moniteur série

  Serial.println("Robot prêt !");
}

void loop()
{
  // Vérification des commandes Bluetooth
  if (Serial.available())
  {
    char commande = Serial.read();

    if (commande == 's')
    {
      modeBluetooth = false;
      arret = true;
      Stop();
      monServo.write(90); // Position du servo au centre
      Serial.println(">> Mode ARRET activé ('s')");
    }
    else if (commande == 'r')
    {
      modeBluetooth = false;
      arret = false;
      Serial.println(">> Mode AUTOMATIQUE activé ('r')");
    }
    else if (commande == 'b')
    {
      modeBluetooth = true;
      arret = false;
      Serial.println(">> Mode BLUETOOTH activé ('b')");
    }
    else if (modeBluetooth)
    {
      // Commandes Bluetooth pour le mode manuel
      switch (commande)
      {
      case 'F': avancer(); break;
      case 'B': reculer(); break;
      case 'L': tournerGauche(0); break;
      case 'R': tournerDroite(0); break;
      default: Stop(); break;
      }
    }
  }

  // Sortir si le mode ARRET ou BLUETOOTH est activé
  if (arret || modeBluetooth)
  {
    return;
  }

    // Mode AUTOMATIQUE
    float distance = lireDistance();

    if (distance > 0 && distance < DISTANCE_TO_CHECK)
    {
        Stop();
        delay(300); // Temps d'arrêt

        // Reculer intelligemment
        reculer();
        delay(400);

        // Scanner plusieurs directions
        monServo.write(0);
        delay(300);
        float distanceGauche = lireDistance();

        monServo.write(90);
        delay(300);
        float distanceCentre = lireDistance();

        monServo.write(180);
        delay(300);
        float distanceDroite = lireDistance();

        // Revenir au centre
        monServo.write(90);
        delay(300);

        // Prise de décision
        if (distanceGauche > DISTANCE_TO_CHECK && distanceDroite > DISTANCE_TO_CHECK)
        {
            avancer();
        }
        else if (distanceGauche > distanceDroite)
        {
            tournerGauche(TURN_DELAY);
        }
        else
        {
            tournerDroite(TURN_DELAY);
        }

        Stop();
        delay(200);
    }
    else
    {
        avancer();
        Serial.println("Aucun obstacle détecté : Avancer");
    }
}

// Fonctions pour les directions

void avancer()
{
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void reculer()
{
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

void tournerGauche(int TURN_DELAY)
{
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  delay(TURN_DELAY);
}

void tournerDroite(int TURN_DELAY)
{
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  delay(TURN_DELAY);
}

void Stop()
{
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

float lireDistance()
{
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duree = pulseIn(ECHO_PIN, HIGH, 30000); // Augmenté à 30 ms
    if (duree == 0)
        return 999; // Valeur élevée si aucun obstacle détecté
    return duree * 0.034 / 2;
}



