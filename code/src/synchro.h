/*  _____   _____ ____    ___   ___ ___  ____
 * |  __ \ / ____/ __ \  |__ \ / _ \__ \|___ \
 * | |__) | |   | |  | |    ) | | | | ) | __) |
 * |  ___/| |   | |  | |   / /| | | |/ / |__ <
 * | |    | |___| |__| |  / /_| |_| / /_ ___) |
 * |_|     \_____\____/  |____|\___/____|____/
 */


#ifndef SYNCHRO_H
#define SYNCHRO_H

#include <QDebug>

#include <pcosynchro/pcosemaphore.h>
#include <pcosynchro/pcothread.h>

#include "locomotive.h"
#include "ctrain_handler.h"
#include "synchrointerface.h"

/**
 * @brief La classe Synchro implémente l'interface SynchroInterface qui
 * propose les méthodes liées à la section partagée.
 */
class Synchro final : public SynchroInterface
{
public:

    /**
     * @brief Synchro Constructeur de la classe qui représente la section partagée.
     * Initialisez vos éventuels attributs ici, sémaphores etc.
     */
    Synchro(): sectionPartagee(1), gare(0), mutex(1), nbrGare(0), numLocoPrioritaire(0), mutexPrio(1), attenteSectionPartagee(0) {

    }

    /**
     * @brief access Méthode à appeler pour accéder à la section partagée
     *
     * Elle doit arrêter la locomotive et mettre son thread en attente si nécessaire.
     *
     * @param loco La locomotive qui essaie accéder à la section partagée
     */
    void access(Locomotive &loco) override {
        // TODO
        mutexPrio.acquire();
        if(numLocoPrioritaire != loco.numero())
        {
            mutexPrio.release();
            loco.arreter();
            attenteSectionPartagee.acquire();
            loco.demarrer();
        }
        else {
            mutexPrio.release();
        }

        sectionPartagee.acquire();


        // Exemple de message dans la console globale
        afficher_message(qPrintable(QString("The engine no. %1 accesses the shared section.").arg(loco.numero())));
    }

    /**
     * @brief leave Méthode à appeler pour indiquer que la locomotive est sortie de la section partagée
     *
     * Réveille les threads des locomotives potentiellement en attente.
     *
     * @param loco La locomotive qui quitte la section partagée
     */
    void leave(Locomotive& loco) override {
        sectionPartagee.release();

        mutexPrio.acquire();
        if(numLocoPrioritaire == loco.numero())
            attenteSectionPartagee.release();
        mutexPrio.release();

        // Exemple de message dans la console globale
        afficher_message(qPrintable(QString("The engine no. %1 leaves the shared section.").arg(loco.numero())));
    }

    /**
     * @brief stopAtStation Méthode à appeler quand la locomotive doit attendre à la gare
     *
     * Implémentez toute la logique que vous avez besoin pour que les locomotives
     * s'attendent correctement.
     *
     * @param loco La locomotive qui doit attendre à la gare
     */
    void stopAtStation(Locomotive& loco) override {
        static const int N_MAX_GARE = 2;
        loco.arreter();
        afficher_message(qPrintable(QString("The engine no. %1 arrives at the station.").arg(loco.numero())));

        mutex.acquire();
        nbrGare++;
        if(nbrGare == N_MAX_GARE)
        {
            mutexPrio.acquire();
            numLocoPrioritaire = loco.numero();
            afficher_message(qPrintable(QString("The engine no. %1 has the priority.").arg(numLocoPrioritaire)));
            mutexPrio.release();

            for (int i = 1; i < N_MAX_GARE; i++) {
                gare.release();
            }
            afficher_message("Trains are released");

            nbrGare = 0;
            mutex.release();
        }
        else
        {
            mutex.release();
            gare.acquire();
        }

        PcoThread::usleep(1*1000*1000);

        loco.demarrer();
    }

    /* A vous d'ajouter ce qu'il vous faut */

private:
    // Méthodes privées ...
    // Attribut privés ...
    PcoSemaphore sectionPartagee;
    PcoSemaphore gare;
    PcoSemaphore mutex;
    int nbrGare;

    int numLocoPrioritaire;
    PcoSemaphore mutexPrio;
    PcoSemaphore attenteSectionPartagee;
};


#endif // SYNCHRO_H
