#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp
//ACTOR//
Actor::Actor(StudentWorld* sWorld, int imgID, double startX, double startY, int startDIR, double size)
: GraphObject(imgID, startX, startY, startDIR, size), m_sWorld(sWorld), m_isAlive(true){}

//ACTORWITHHP//
ActorWithHP::ActorWithHP(StudentWorld* sWorld, int imgID, double startX, double startY, int startDIR, double size, int hp)
: Actor(sWorld, imgID, startX, startY, startDIR, size){
    m_hp = hp;
    setVisible(true);
}

void ActorWithHP::damage(){
    decHitPoints(2); //hit by pea
    
    if (getHitPoints() <= 0){ //if hp is 0 or below
        die();
        getWorld()->increaseScore(100); //increase score
    }
}

void ActorWithHP::shoot(){
    int direction = getDirection();
    double x = this->getX();
    double y = this->getY();
    switch(direction){
        case up:
            getWorld()->addPea(x, y + 1, up); //add pea one above if looking up
            break;
        case down:
            getWorld()->addPea(x, y - 1, down); //add pea one below if looking down
            break;
        case left:
            getWorld()->addPea(x - 1, y, left); //add pea x-1 (left) if looking left
            break;
        case right:
            getWorld()->addPea(x+1, y, right); //add pea x+1 (right) if looking right
            break;
    }
}
//AVATAR//
Avatar::Avatar(StudentWorld* sWorld, double startX, double startY): ActorWithHP(sWorld, IID_PLAYER, startX, startY, right, 1.0, 20){
    m_peas = 20; //starts with 20 peas to shoot
    setVisible(true);
}


void Avatar::doSomething(){
    if (!alive()) return; //if not alive, do nothing
    int input;
    if(getWorld()->getKey(input)){

        switch(input){
            case KEY_PRESS_UP: //up
                if(getWorld()->canMoveTo(getX(), getY()+1)){ //if can move y+1,
                    if (getWorld()->isMarble(getX(), getY()+1) && getWorld()->canMarbleMoveTo(getX(), getY()+2)){ //if there is a marble and the marble can move, move
                        Actor* ptr = getWorld()->getActorAt(getX(), getY()+1);
                        if (ptr != nullptr){
                            ptr->moveTo(getX(), getY()+2); //move marble
                            moveTo(getX(), getY()+1); //move player
                            setDirection(up); //change direction
                        }
                    }
                    else if (getWorld()->isMarble(getX(), getY()+1) && !getWorld()->canMarbleMoveTo(getX(), getY()+2)){ //change direction if neither can move
                        setDirection(up);
                        break;
                    }
                    else{
                        moveTo(getX(), getY()+1); //otherwise just move itself
                        setDirection(up);
                    }
                }
                else{
                    setDirection(up); //change dir when nothing can move
                }
                break;
            case KEY_PRESS_DOWN:
                if(getWorld()->canMoveTo(getX(), getY()-1)){ //if can move down
                    if (getWorld()->isMarble(getX(), getY()-1) && getWorld()->canMarbleMoveTo(getX(), getY()-2)){ //handles marbles when the marble can move
                        Actor* ptr = getWorld()->getActorAt(getX(), getY()-1);
                        if (ptr != nullptr){
                            ptr->moveTo(getX(), getY()-2); //move marble
                            moveTo(getX(), getY()-1); //move player
                            setDirection(down); //change direction
                        }
                    }
                    else if (getWorld()->isMarble(getX(), getY()-1) && !getWorld()->canMarbleMoveTo(getX(), getY()-2)){ //wehn the marble can't move
                        setDirection(down);
                        break;
                    }
                    else{
                        moveTo(getX(), getY()-1); //move when there is no marble
                        setDirection(down);
                    }
                }
                else{
                    setDirection(down); //otherwise just change direction of avatar
                }
                break;
            case KEY_PRESS_LEFT:
                if(getWorld()->canMoveTo(getX()-1, getY())){ //if it can move left,
                    if (getWorld()->isMarble(getX()-1, getY()) && getWorld()->canMarbleMoveTo(getX()-2, getY())){ //handles when there is a marble to the left
                        Actor* ptr = getWorld()->getActorAt(getX()-1, getY());
                        if (ptr != nullptr){ //refrain from accessing nullptr
                            ptr->moveTo(getX()-2, getY()); //move marble
                            moveTo(getX()-1, getY()); //move player
                            setDirection(left);
                        }
                    }
                    else if (getWorld()->isMarble(getX()-1, getY()) && !getWorld()->canMarbleMoveTo(getX()-2, getY())){
                        setDirection(left); //change direction when the marble cannot move
                        break;
                    }
                    else{
                        moveTo(getX()-1, getY()); //move itself
                        setDirection(left);
                    }
                }
                else{
                    setDirection(left); //change dir otherwise
                }
                break;
            case KEY_PRESS_RIGHT:
                if(getWorld()->canMoveTo(getX()+1, getY())){
                    if (getWorld()->isMarble(getX()+1, getY()) && getWorld()->canMarbleMoveTo(getX()+2, getY())){ //handles when the marble can move
                        Actor* ptr = getWorld()->getActorAt(getX()+1, getY());
                        if (ptr != nullptr){
                            ptr->moveTo(getX()+2, getY()); //move marble
                            moveTo(getX()+1, getY()); //move itself
                            setDirection(right); //change dir
                        }
                    }
                    else if (getWorld()->isMarble(getX()+1, getY()) && !getWorld()->canMarbleMoveTo(getX()+2, getY())){
                        setDirection(right); //change dir if marble can't move
                        break;
                    }
                    else{
                        moveTo(getX()+1, getY()); //move itself
                        setDirection(right);
                    }
                }
                else{
                    setDirection(right); //change dir
                }
                break;
            case KEY_PRESS_SPACE:
                if (m_peas > 0){ //if player still ahs ammo
                    shoot();
                    getWorld()->playSound(SOUND_PLAYER_FIRE);
                    m_peas--; //pea was shot
                }
                break;
            case KEY_PRESS_ESCAPE:
                die(); //lose a life
                break;
        }
    }
}

void Avatar::damage(){
    ActorWithHP::damage(); //handles the same when pea is hit, -2 hp
    getWorld()->playSound(SOUND_PLAYER_IMPACT);
}

void Avatar::die(){
    Actor::die(); //will be handled by studentwolrd
    getWorld()->playSound(SOUND_PLAYER_DIE);
}

//WALL//
Wall::Wall(StudentWorld* sWorld, double startX, double startY): Actor(sWorld, IID_WALL, startX, startY, 0, 1.0){
    setVisible(true); //set visible when created
}

//MARBLE//
Marble::Marble(StudentWorld* sWorld, double startX, double startY): ActorWithHP(sWorld, IID_MARBLE, startX, startY, 0, 1.0, 10){
    setVisible(true); //set visible when created
}

void Marble::doSomething(){
    return; //do nothing, it is a marble
}

//RAGEBOT//
RageBot::RageBot(StudentWorld* sWorld, double startX, double startY, int dir): Robot(sWorld, startX, startY, IID_RAGEBOT, dir, 10){setVisible(true);} //set visible when created

void RageBot::doSomething(){
    if (!alive() || !correctTick()) return;
    
    if (getWorld()->hasClearShot(getX(), getY(), getDirection())){ //if it has a clear shot
        shoot(); //then shoot
        getWorld()->playSound(SOUND_ENEMY_FIRE); //sound
    }
    else{ //then move if it does not have a clear shot
        int dir = getDirection();
        switch (dir){ //first two cases are horizontal ragebots, last two are for vertical ragebots
            case left:
                if (getWorld()->canRobotMoveTo(getX()-1, getY())){ //if robot can move left, move left
                    moveTo(getX()-1, getY());
                }
                else{
                    setDirection(right); //otherwise, turn right bc it hit a wall or something that it cannot colocate with
                    break;
                }
                break;
            case right:
                if (getWorld()->canRobotMoveTo(getX()+1, getY())){ //if can move right, move right
                    moveTo(getX()+1, getY());
                }
                else{
                    setDirection(left); //otherwise, turn left bc it hit a wall or something that it cannot colocate with
                    break;
                }
                break;
            case up:
                if (getWorld()->canRobotMoveTo(getX(), getY()+1)){ //if can move up, keep moving up
                    moveTo(getX(), getY()+1);
                }
                else{
                    setDirection(down); //otherwise, turn downwards bc it hit a wall or something that it cannot colocate with
                    break;
                }
                break;
            case down:
                if (getWorld()->canRobotMoveTo(getX(), getY()-1)){ //if can move down, move down
                    moveTo(getX(), getY()-1);
                }
                else{
                    setDirection(up); //otherwise, turn up bc it hit a wall or something that it cannot colocate with
                    break;
                }
                break;
            default:
                break;
        }
    }
}

void RageBot::damage(){
    decHitPoints(2); //hit by pea
    getWorld()->playSound(SOUND_ROBOT_IMPACT); //sound
    
    if (getHitPoints() <= 0){ //if the pea caused it to die, die
        die();
        getWorld()->playSound(SOUND_ROBOT_DIE);
        getWorld()->increaseScore(100); //increase score
    }
}

//THIEF BOT//
ThiefBot::ThiefBot(StudentWorld* sWorld, double startX, double startY, int imgID, int dir, int hp): Robot(sWorld, startX, startY, imgID, dir, hp){
    setVisible(true); //visible when created
    m_holdingObject = false; //does not start with a goodie
    m_distBeforeTurning = randInt(1, 6); //set distance before turning
    m_goodie = nullptr; //does not start with a goodie
}

void ThiefBot::doSomething(){
//    if (!alive() || !correctTick()) return; //fiinish
    
    if (getWorld()->goodieOnSquare(getX(), getY()) && getGoodieHolding() == nullptr){ //pick up goodie if not holding one
        int pickupChance = randInt(0, 9); //1/10 chance of picking something up
        if (pickupChance == 5 && getGoodieHolding() == nullptr){ //pick up
            if (getWorld()->getGoodieAt(this->getX(), this->getY()) != nullptr){
                setGoodieHolding(getWorld()->getGoodieAt(this->getX(), this->getY())); //holds pointer to goodie
                getWorld()->getGoodieAt(getX(), getY())->setVisible(false); //makes goodie invisble thus cannot be picked up by player
                getWorld()->playSound(SOUND_ROBOT_MUNCH);
                return;
            }
        }
    }
    if (getDistTraveled() < getDistBeforeTurning()){ //if distance traveled is less than what they r supposed to travel b4 turning
        int dir = getDirection();
        switch (dir){
            case left:
                if (getWorld()->canRobotMoveTo(getX()-1, getY())){ //go left
                    moveTo(getX()-1, getY());
                    incDistTraveled(); //dist traveled +1
                    return;
                }
                break;
            case right:
                if (getWorld()->canRobotMoveTo(getX()+1, getY())){ //right
                    moveTo(getX()+1, getY());
                    incDistTraveled(); //dist traveled +1
                    return;
                }
                break;
            case up:
                if (getWorld()->canRobotMoveTo(getX(), getY()+1)){ //up
                    moveTo(getX(), getY()+1);
                    incDistTraveled(); //dist traveled +1
                    return;
                }
                break;
            case down:
                if (getWorld()->canRobotMoveTo(getX(), getY()-1)){ //downn
                    moveTo(getX(), getY()-1);
                    incDistTraveled(); //dist traveled +1
                    return;
                }
                break;
            default:
                break;
        }
    }
    
    int currDir = getDirection();
    setDistBeforeTurning(randInt(1, 6)); //new dist before turning
    setDistTraveled(0); //since reset, dist traveled = 0

    bool discoveredDirections[4] = {false, false, false, false}; //use for not reconsidering location

    bool foundDirection = false;

    while (!foundDirection) { //while a valid direction is not found
        int directionRand = randInt(0, 3); //used to access array of bools
        switch (directionRand) {
            case 0: // up
                if (!discoveredDirections[0] && getWorld()->canThiefBotMoveTo(getX(), getY(), 0, 1)) { //if not already considered and can move to
                    moveTo(getX(), getY() + 1);
                    setDirection(up);
                    incDistTraveled();
                    foundDirection = true; //found direction, will break at the end
                } else {
                    discoveredDirections[0] = true; //considered
                }
                break;
            case 1: // down
                if (!discoveredDirections[1] && getWorld()->canThiefBotMoveTo(getX(), getY(), 0, -1)) { //if not alrdy considrered and can move to
                    moveTo(getX(), getY() - 1);
                    setDirection(down);
                    incDistTraveled();
                    foundDirection = true; //found direction, will break at the end
                } else {
                    discoveredDirections[1] = true; //considered
                }
                break;
            case 2: // right
                if (!discoveredDirections[2] && getWorld()->canThiefBotMoveTo(getX(), getY(), 1, 0)) {
                    moveTo(getX() + 1, getY());
                    setDirection(right);
                    incDistTraveled();
                    foundDirection = true; //found direction, will break at the end
                } else {
                    discoveredDirections[2] = true; //considered
                }
                break;
            case 3: // left
                if (!discoveredDirections[3] && getWorld()->canThiefBotMoveTo(getX(), getY(), -1, 0)) {
                    moveTo(getX() - 1, getY());
                    setDirection(left);
                    incDistTraveled();
                    foundDirection = true; //found direction, will break at the end
                } else {
                    discoveredDirections[3] = true; //considered
                }
                break;
        }
        if (!foundDirection) {
            // if no valid direction, just keep the current direction
            setDirection(currDir);
            return;
        }
    }

}

void ThiefBot::damage(){
    decHitPoints(2); //hit by pea
    getWorld()->playSound(SOUND_ROBOT_IMPACT);
    
    if (getHitPoints() <= 0){ //if dead,
        if(getGoodieHolding() != nullptr){ //if it has a goodie
            getGoodieHolding()->moveTo(getX(), getY()); //move goodie to where it died
            getGoodieHolding()->setVisible(true); //make goodie visible and able to be picked up
        }
        die(); //delete off screen
        getWorld()->playSound(SOUND_ROBOT_DIE);
        getWorld()->increaseScore(10);
    }
}

//REGULAR THIEF BOT//

RegularThiefBot::RegularThiefBot(StudentWorld* sWorld, double startX, double startY, int dir): ThiefBot(sWorld, startX, startY, IID_THIEFBOT, dir, 5){
    setVisible(true);
//    m_distBeforeTurning = randInt(1, 6);
}

void RegularThiefBot::doSomething(){
    if (!alive() || !correctTick()) return; //fiinish
    else{
        ThiefBot::doSomething(); //same thing as thiefbot base class
    }
}

//MEAN THIEF BOT//
MeanThiefBot::MeanThiefBot(StudentWorld* sWorld, double startX, double startY, int dir): ThiefBot(sWorld, startX, startY, IID_MEAN_THIEFBOT, dir, 8){
    setVisible(true);
}

void MeanThiefBot::doSomething(){
    if (!alive() || !correctTick()) return; //fiinish
    
    if (getWorld()->hasClearShot(getX(), getY(), getDirection())){ //if it has clear shot, then shoot bc mean rage bot
        shoot();
        getWorld()->playSound(SOUND_ENEMY_FIRE);
    }
    else{ //if no clear shot, just do what a regular thief bot would do
        ThiefBot::doSomething();
    }
}

void MeanThiefBot::damage(){
    decHitPoints(2); //damaged by pea
    getWorld()->playSound(SOUND_ROBOT_IMPACT);
    
    if (getHitPoints() <= 0){ //if it dies
        if(getGoodieHolding() != nullptr){ //if it has a goodie
            getGoodieHolding()->moveTo(getX(), getY());
            getGoodieHolding()->setVisible(true); //set goodie visible after moving it to location where thief bot died
        }
        die();
        getWorld()->playSound(SOUND_ROBOT_DIE);
        getWorld()->increaseScore(20); //increase score by 20 instead of 10
    }
}

//GOODIE//
Goodie::Goodie(StudentWorld* sWorld, int imgID, double startX, double startY):Actor(sWorld, imgID, startX, startY, 0, 1.0){
    setVisible(true);
}

void Goodie::doSomething(){
    if (!alive()) return;
    
    if (getWorld()->playerIsOnSquare(getX(), getY()) && isVisible()){ //if player on top of goodie
        goodieEffect(); //goodie effect diff for each one
    }
}

//CRYSTAL//
Crystal::Crystal(StudentWorld* sWorld, double startX, double startY): Goodie(sWorld, IID_CRYSTAL, startX, startY){
    setVisible(true);
}

void Crystal::goodieEffect(){
    getWorld()->increaseScore(50);
    die(); //delete
    getWorld()->decCrystals(); //notify student world that one crystal gone
    getWorld()->playSound(SOUND_GOT_GOODIE);
}

//EXTRALIFE//
ExtraLife::ExtraLife(StudentWorld* sWorld, double startX, double startY): Goodie(sWorld, IID_EXTRA_LIFE, startX, startY){
    setVisible(true);
}

void ExtraLife::goodieEffect(){
    getWorld()->increaseScore(1000);
    getWorld()->incLives(); //m_lives++
    die(); //delete
    getWorld()->playSound(SOUND_GOT_GOODIE);
}

//RESTOREHEALTH//
RestoreHealth::RestoreHealth(StudentWorld* sWorld, double startX, double startY): Goodie(sWorld, IID_RESTORE_HEALTH, startX, startY){
    setVisible(true);
}

void RestoreHealth::goodieEffect(){
    getWorld()->increaseScore(500);
    die(); //delete
    getWorld()->playSound(SOUND_GOT_GOODIE);
    getWorld()->getAvatar()->setHitPoints(20); //full hp = 20
}
//AMMO//
Ammo::Ammo(StudentWorld* sWorld, double startX, double startY): Goodie(sWorld, IID_AMMO, startX, startY){
    setVisible(true);
}

void Ammo::goodieEffect(){
    getWorld()->increaseScore(100);
    die(); //delete
    getWorld()->playSound(SOUND_GOT_GOODIE);
    getWorld()->getAvatar()->increasePeas(20); //ammo += 20
}
//PIT//
Pit::Pit(StudentWorld* sWorld, double startX, double startY): Actor(sWorld, IID_PIT, startX, startY, 0, 1.0){
    setVisible(true);
}

void Pit::doSomething(){
    if (!alive()) return;
    
    if (getWorld()->isMarble(getX(), getY())){
        getWorld()->getMarble(getX(), getY())->die(); //if marble is on top, delete pit
        die();
        //get rid of marble here
    }
}
 //PEA//
Pea::Pea(StudentWorld* sWorld, double startX, double startY, int dir): Actor(sWorld, IID_PEA, startX, startY, dir, 1.0){
    setVisible(true);
}

void Pea::doSomething(){
    if (!alive()) return;
    
    if (getWorld()->overlappedWithDamageableActor(getX(), getY())){ //if on damageable actor, damage the actor
        getWorld()->getDamageableActor(getX(), getY())->damage();
        die();
    }
    else if (getWorld()->playerIsOnSquare(getX(), getY())){ //if player is on sqaure, damage player
        getWorld()->getAvatar()->damage();
        die();
    }
    else if (getWorld()->overlappedWithStopsPea(getX(), getY())){ //if it blocks pea, like wall, delete pea
        die();
    }
    
    if (!alive()) return;
    
    int direction = getDirection();
    switch(direction){
        case up:
            moveTo(getX(), getY()+1); //keep moving up
            break;
        case down:
            moveTo(getX(), getY()-1); //keep moving down
            break;
        case left:
            moveTo(getX()-1, getY()); //keep moving left
            break;
        case right:
            moveTo(getX()+1, getY()); //keep moving right
            break;
    }
    
    if (getWorld()->overlappedWithDamageableActor(getX(), getY())){ //repeat of steps above because of #2 of pea in spec
        getWorld()->getDamageableActor(getX(), getY())->damage();
        die();
    }
    else if (getWorld()->playerIsOnSquare(getX(), getY())){
        getWorld()->getAvatar()->damage();
        die();
    }
    else if (getWorld()->overlappedWithStopsPea(getX(), getY())){
        die();
    }
}

//EXIT//
Exit::Exit(StudentWorld *sWorld, double startX, double startY):Actor(sWorld, IID_EXIT, startX, startY, 0, 1.0){
    setVisible(false);
}

void Exit::doSomething(){
    if (getWorld()->allCrystalsCollected() && !isVisible()){
        setVisible(true);
        getWorld()->playSound(SOUND_REVEAL_EXIT);
    }
    if (isVisible() && getWorld()->playerIsOnSquare(getX(), getY())){ //if player is on exit
        getWorld()->playSound(SOUND_FINISHED_LEVEL);
        getWorld()->increaseScore(2000);
        getWorld()->increaseScore(getWorld()->getBonus()); //handles bonus
        getWorld()->setLevelFinished(); //let student world know that player is done with level
    }
}

//FACTORY//
ThiefBotFactory::ThiefBotFactory(StudentWorld* sWorld, int startX, int startY, ProductType type): Actor(sWorld, IID_ROBOT_FACTORY, startX, startY, 0, 1.0){
    setVisible(true);
    m_type = type;
}

void ThiefBotFactory::doSomething(){
    int censusCount = getWorld()->thiefBotCensus(getX(), getY(), 3);
    
    if (censusCount < 3 && !getWorld()->thiefBotHere(getX(), getY())){ //if there are less than 3 thiefbots around radius
        int chance = randInt(1, 50);
        if (chance == 1){
            if (getType() == MEAN){ //if it is a mean thiefbot factory
                getWorld()->addMeanThiefBot(getX(), getY(), right);
            }
            else{ //regular thief bot
                getWorld()->addRegularThiefBot(getX(), getY(), right);
            }//fix direction later!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            getWorld()->playSound(SOUND_ROBOT_BORN);
        }
    }
    else{
        return;
    }
}


//ROBOT//
Robot::Robot(StudentWorld* sWorld, double startX, double startY, int imgID, int dir, int hp): ActorWithHP(sWorld, imgID, startX, startY, dir, 1.0, hp){
    m_tick = (28 - (getWorld()->getLevel())) / 4; // levelNumber is the current
     // level number (0, 1, 2, etc.)
    if (m_tick < 3)
     m_tick = 3; // no RageBot moves more frequently than this
    
    m_nTicks = 1; //ticks will always start at 1
}

//TICK//
bool Robot::correctTick(){
    if (m_tick == m_nTicks){ //if tick is the correct tick
        m_nTicks = 1; //reset ticks
        return true; //is the right tick
    }
    m_nTicks++; //otherwise keep increasing ticks
    return false; //not the correct tick
}
