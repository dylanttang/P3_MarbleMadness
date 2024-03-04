#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
//#include "StudentWorld.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class StudentWorld;

class Actor: public GraphObject {
public:
        Actor(StudentWorld* sWorld, int imgID, double startX, double startY, int startDIR, double size);
        virtual void doSomething() = 0; //never call actor dosmth
        StudentWorld* getWorld(){ return m_sWorld; } //getWorld()->
        virtual bool moveable() { return false;} //can be moved
        bool alive() { return m_isAlive; } //is it alive
        virtual void die() { m_isAlive = false; /*setVisible(false); */} //will become invisible in student world
        virtual bool allowsAgentColocation() const{ return false; } //stackable?
        virtual bool allowsMarble() const { return false; } //for pit
//        virtual bool countsInFactoryCensus() const;
        virtual bool stopsPea() const { return true; } //pea does not go through
        virtual bool isDamageable() const { return false; } //can be damaged by pea
//        virtual bool bePushedBy(Actor* a, int x, int y);
        virtual bool isSwallowable() const { return false; } //can take marble
        virtual bool isStealable() const { return false; } //can be picked up by thief bots, for goodies
        virtual void damage() /*{ return; }*/ = 0; //base class actor never damaged, each has own sound
        virtual bool robotColocation() const { return false; } //can robot be on top of
        virtual bool isRobot() { return false; }
        virtual bool robotcanShootThrough() { return false; } //can robot shoot through this? for pit.
        virtual bool isPea() {return false; }
        virtual bool isThiefBot() {return false; }
        virtual bool pickupable() { return false; } //can be picked up by thief bot
//        virtual bool tryToBeKilled(int damageAmt);
private:
        StudentWorld* m_sWorld;
        bool m_isAlive; //to check if it is alive in student world
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Wall: public Actor {
public:
        Wall(StudentWorld* sWorld, double startX, double startY);
        virtual void doSomething(){ return; } //do nothing it is a wall
        virtual void damage() { return; } //do nothing it is a wall
    
private:
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////

class ActorWithHP: public Actor {
public:
        ActorWithHP(StudentWorld* sWorld, int imgID, double startX, double startY, int startDIR, double size, int hp);
        virtual void doSomething() = 0;
        bool isAlive() { return m_hp <= 0; }
        virtual int getHitPoints() const { return m_hp; }
        virtual void setHitPoints(int amt){ m_hp = amt; }
        void decHitPoints(int amt){ m_hp -= amt; }
        virtual bool isDamageable() const { return true; }
        virtual void damage(); /*{ m_hp -= 2; }*/
        virtual void shoot();
    
private:
        int m_hp;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////

class Avatar: public ActorWithHP {
public:
        Avatar(StudentWorld* sWorld, double startX, double startY);
        virtual void doSomething();
        virtual void die();
        int getAmmo() { return m_peas;}
        void increasePeas(int amt) { m_peas += amt; }
        virtual bool correctTick() { return true; }
        int getHPPercent() { return getHitPoints()/.2; }
        virtual void damage();
    
private:
        int m_peas;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////

class Marble: public ActorWithHP {
public:
        Marble(StudentWorld* sWorld, double startX, double startY);
        virtual void doSomething();
        virtual bool moveable() {return true;}
        virtual bool isSwallowable() const { return true; }
        virtual bool isDamageable() const { return true; }
        virtual bool allowsAgentColocation() const{ return true; }
    
private:
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////

class Robot: public ActorWithHP {
public:
        Robot(StudentWorld* sWorld, double startX, double startY, int imgID, int dir, int hp);
        virtual void doSomething() { return; }
        virtual bool correctTick();
private:
        int m_tick;
        int m_nTicks;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////

class RageBot: public Robot {
public:
        RageBot(StudentWorld* sWorld, double startX, double startY, int dir);
        virtual void doSomething();
        virtual void damage();
        virtual bool isRobot() { return true; }
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////

class ThiefBot: public Robot {
public:
        ThiefBot(StudentWorld* sWorld, double startX, double startY, int imgID, int dir, int hp);
        virtual void doSomething();
        virtual bool isRobot() { return true; }
        virtual bool isThiefBot() {return true; }
        Actor* getGoodieHolding() { return m_goodie; }
        void setGoodieHolding(Actor* goodie) { m_goodie = goodie; }
        int getDistBeforeTurning() { return m_distBeforeTurning; }
        int getDistTraveled() { return m_distTraveled; }
        void setDistTraveled(int amt) { m_distTraveled = amt; }
        void incDistTraveled() { m_distTraveled++; }
        virtual void damage();
        void setDistBeforeTurning(int amt) { m_distBeforeTurning = amt; }
private:
        int m_distBeforeTurning;
        int m_distTraveled;
        bool m_holdingObject;
        Actor* m_goodie;
        
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////

class RegularThiefBot: public ThiefBot {
public:
        RegularThiefBot(StudentWorld* sWorld, double startX, double startY, int dir);
        virtual void doSomething();
        virtual bool isRobot() { return true; }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

class MeanThiefBot: public ThiefBot {
public:
        MeanThiefBot(StudentWorld* sWorld, double startX, double startY, int dir);
        virtual void doSomething();
        virtual bool isRobot() { return true; }
        virtual void damage();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Goodie: public Actor{
public:
        Goodie(StudentWorld* sWorld, int imgID, double startX, double startY);
        virtual void doSomething();
        virtual bool isStealable() const { return true; }
        virtual bool allowsAgentColocation() const{ return true; }
        virtual bool stopsPea() const { return false; }
        virtual void damage() { return; }
        virtual bool robotColocation() const { return true; }
        virtual bool robotcanShootThrough() { return true; }
        virtual bool pickupable() { return true; }
    
protected:
        virtual void goodieEffect() = 0;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Crystal: public Goodie {
public:
        Crystal(StudentWorld* sWorld, double startX, double startY);
        virtual void doSomething(){ Goodie::doSomething(); }
        virtual bool isStealable() const { return false; }
        virtual bool pickupable() { return false; }
    
protected:
        virtual void goodieEffect();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ExtraLife: public Goodie {
public:
        ExtraLife(StudentWorld* sWorld, double startX, double startY);
        virtual void doSomething(){ Goodie::doSomething(); }
    
protected:
        virtual void goodieEffect();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RestoreHealth: public Goodie {
public:
        RestoreHealth(StudentWorld* sWorld, double startX, double startY);
        virtual void doSomething(){ Goodie::doSomething(); }
    
protected:
        virtual void goodieEffect();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Ammo: public Goodie{
public:
        Ammo(StudentWorld* sWorld, double startX, double startY);
        virtual void doSomething(){ Goodie::doSomething(); }
    
protected:
        virtual void goodieEffect();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Pit: public Actor {
public:
        Pit(StudentWorld* sWorld, double startX, double startY);
        virtual void doSomething();
        virtual bool allowsMarble() const { return true; }
        virtual bool stopsPea() const { return false; }
        virtual void damage() { return; }
        virtual bool robotcanShootThrough() { return true; }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Pea: public Actor {
public:
        Pea(StudentWorld* sWorld, double startX, double startY, int dir);
        virtual void doSomething();
        virtual bool stopsPea() const { return false; }
        virtual void damage() { return; }
        virtual bool robotColocation() const { return true; }
        virtual bool isPea() {return true; }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Exit: public Actor {
public:
    Exit(StudentWorld* sWorld, double startX, double startY);
    virtual void doSomething();
    virtual bool allowsAgentColocation() const{ return true; }
    virtual bool stopsPea() const { return false; }
    virtual void damage() { return; }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ThiefBotFactory : public Actor
{
public:
        enum ProductType { REGULAR, MEAN }; //different types

        ThiefBotFactory(StudentWorld* sWorld, int startX, int startY, ProductType type);
        virtual void doSomething();
        virtual void damage() { return; }
        ProductType getType() { return m_type; }
    
private:
    ProductType m_type; //holds what type of factory it is
};
#endif // ACTOR_H_
