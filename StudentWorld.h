#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"
#include <string>


// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
#include "Actor.h"
#include <vector>
#include "GraphObject.h"
#include <iostream> // defines the overloads of the << operator
#include <sstream>  // defines the type std::ostringstream
#include <iomanip>

using namespace std;

class StudentWorld : public GameWorld
{
public:
        StudentWorld(std::string assetPath);
        virtual int init();
        virtual int move();
        virtual void cleanUp();
    
        ~StudentWorld();
        bool canMoveTo(double x, double y);
        bool allCrystalsCollected(){return m_nCrystals == 0;}
        bool playerIsOnSquare(double x, double y) const;
        bool emptySquare(double x, double y) const;
        Avatar* getAvatar() { return m_Avatar; }
        void setLevelFinished() { m_completedLvl = true; }
        bool isMarble(double x, double y);
        Actor* getMarble(double x, double y);
        Actor* getActorAt(double x, double y);
        void decCrystals(){ m_nCrystals--; }
        bool overlappedWithDamageableActor(double x, double y);
        Actor* getDamageableActor(double x, double y);
        bool overlappedWithStopsPea(double x, double y);
        bool playerSameRow (double y);
        bool playerSameCol (double x);
        bool canRobotMoveTo(double x, double y);
        bool hasClearShot (double x, double y, int dir);
        bool thiefBotHere (double x, double y);
        int thiefBotCensus(double x, double y, int dist);
        void setDisplayText();
        int getBonus() { return m_bonus; }
        bool goodieOnSquare(double x, double y);
        Actor* getGoodieAt(double x, double y);
          // Can a marble move to x,y?
        bool canMarbleMoveTo(double x, double y) const;
          // Is the player on the same square as an Actor?
        bool isPlayerColocatedWith(int x, int y) const;
          // If an item that can be stolen is at x,y, return a pointer to it;
          // otherwise, return a null pointer.  (Stealable items are only ever
          // going be goodies.)
        Actor* getColocatedStealable(int x, int y) const;
          // Restore player's health to the full amount.
        void restorePlayerHealth(){ m_Avatar->setHitPoints(20);}
          // Increase the amount of ammunition the player has
        void increaseAmmo();
    
    // Note:  A convention used in a number of interfaces is to represent a
    // direction with the adjustments to x and y needed to move one step in
    // that direction:
    // left:   dx is -1, dy is  0
    // right:  dx is  1, dy is  0
    // up:     dx is  0, dy is  1
    // down:   dx is  0, dy is -1
    // Can agent move to x,y?  (dx and dy indicate the direction of motion)
        bool canThiefBotMoveTo(double x, double y, double dx, double dy) const;
    
    
          // Add an actor to the world
        void addPea(double x, double y, int dir);
        void addRegularThiefBot(double x, double y, int dir);
        void addMeanThiefBot(double x, double y, int dir);

private:
        Avatar* m_Avatar;
        vector<Actor*> m_actors;
        bool m_completedLvl;
        int m_nCrystals;
        int m_ticks; //ticks
        int m_bonus; //for the bonus
};

#endif // STUDENTWORLD_H_
