/*  _____   _____ ____    ___   ___ ___  ____
 * |  __ \ / ____/ __ \  |__ \ / _ \__ \|___ \
 * | |__) | |   | |  | |    ) | | | | ) | __) |
 * |  ___/| |   | |  | |   / /| | | |/ / |__ <
 * | |    | |___| |__| |  / /_| |_| / /_ ___) |
 * |_|     \_____\____/  |____|\___/____|____/
 */

#include "locomotivebehavior.h"
#include "ctrain_handler.h"
#include "synchro.h"

void LocomotiveBehavior::run()
{
    //Initialisation de la locomotive
    loco.allumerPhares();
    loco.demarrer();
    loco.afficherMessage("Ready!");

    while(true) {

        if(loco.numero() == NUM_LOCO_A)
        {
            //Commence a 34

            //Se raproche de la gare
            attendre_contact(34);
            loco.fixerVitesse(loco.vitesse()/FACTEUR_REDUCTION_VITESSE);

            //Va a la gare 33
            attendre_contact(33);
            sharedSection->stopAtStation(loco);

            //Debut de la section partagée
            attendre_contact(25);
            sharedSection->access(loco);
            diriger_aiguillage(8, DEVIE, 0);
            loco.fixerVitesse(loco.vitesse()*FACTEUR_REDUCTION_VITESSE);

            //Fin de la section partagée
            attendre_contact(14);
            sharedSection->leave(loco);

            //Repasse par 34 et recommence
        }
        else if(loco.numero() == NUM_LOCO_B)
        {
            //Commence a 31

            //Se raproche de la gare
            attendre_contact(30);
            loco.fixerVitesse(loco.vitesse()/FACTEUR_REDUCTION_VITESSE);

            //Va a la gare 29
            attendre_contact(29);
            sharedSection->stopAtStation(loco);

            //Debut de la section partagée
            attendre_contact(22);
            sharedSection->access(loco);
            diriger_aiguillage(8, TOUT_DROIT, 0);
            loco.fixerVitesse(loco.vitesse()*FACTEUR_REDUCTION_VITESSE);

            //Fin de la section partagée
            attendre_contact(10);
            sharedSection->leave(loco);

            //Repasse par 31 et recommence
        }
    }
}

void LocomotiveBehavior::printStartMessage()
{
    qDebug() << "[START] Thread de la loco" << loco.numero() << "lancé";
    loco.afficherMessage("Je suis lancée !");
}

void LocomotiveBehavior::printCompletionMessage()
{
    qDebug() << "[STOP] Thread de la loco" << loco.numero() << "a terminé correctement";
    loco.afficherMessage("J'ai terminé");
}
