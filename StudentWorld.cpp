#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    m_Avatar = nullptr; //avatar null
    //clear actors here
    m_actors.clear();
    m_nCrystals = 0;
    m_ticks = 0;
    m_bonus = 1000; //bonus starts at 1000
}

int StudentWorld::init()
{
    m_completedLvl = false; //lets it read in other levels
    int levelnumber = getLevel();
    string lvlFileName;
    if (levelnumber < 9){ //when the level is less than 2 digits
         lvlFileName = "level0" + to_string(getLevel()) + ".txt";
    }
    else { //when it is 2 digits, there is no 0 in front
         lvlFileName = "level" + to_string(getLevel()) + ".txt";
    }
    Level lvl(assetPath());
    Level::LoadResult result = lvl.loadLevel(lvlFileName);
    if (result == Level::load_fail_file_not_found){
        cerr << "Could not find level00.txt data file\n";
        return GWSTATUS_LEVEL_ERROR;
    }
    else if (result == Level::load_fail_bad_format){
        cerr << "Your level was improperly formatted\n";
        return GWSTATUS_LEVEL_ERROR;
    }
    else if (result == Level::load_success){
        cerr << "Successfully loaded level\n";
//        return GWSTATUS_LEVEL_ERROR;
    }
    
    for(int x = 0; x < 16 ; x++){
        for(int y = 0; y < 16; y++){
            Level::MazeEntry item = lvl.getContentsOf(x, y);
            
            switch (item){
                case Level::player: //player
                    m_Avatar = new Avatar(this, x, y);
                    break;
                case Level::wall: //wall
                    m_actors.push_back(new Wall(this, x, y));
                    break;
                case Level::marble: //marble
                    m_actors.push_back(new Marble(this, x, y));
                    break;
                case Level::vert_ragebot: //vert rage bot starts upwards always
                    m_actors.push_back(new RageBot(this, x, y, GraphObject::down));
                    break;
                case Level::horiz_ragebot: //vert rage bot starts rightwards always
                    m_actors.push_back(new RageBot(this, x, y, GraphObject::right));
                    break;
                case Level::pit: //put
                    m_actors.push_back(new Pit(this, x, y));
                    break;
                case Level::crystal: //crystal
                    m_actors.push_back(new Crystal(this, x, y));
                    m_nCrystals++; //whenever a crystal is read in, add it to the number of crystals
                    break;
                case Level::extra_life: //extra life goodie
                    m_actors.push_back(new ExtraLife(this, x, y));
                    break;
                case Level::restore_health: //restore health goodie
                    m_actors.push_back(new RestoreHealth(this, x, y));
                    break;
                case Level::ammo: //ammo goodie
                    m_actors.push_back(new Ammo(this, x, y));
                    break;
                case Level::exit: //exit
                    m_actors.push_back(new Exit(this, x, y));
                    break;
                case Level::thiefbot_factory: //regular thiefbot factory
                    m_actors.push_back(new ThiefBotFactory(this, x, y, ThiefBotFactory::REGULAR));
                    break;
                case Level::mean_thiefbot_factory: //mean thiefbot factory
                    m_actors.push_back(new ThiefBotFactory(this, x, y, ThiefBotFactory::MEAN));
                    break;
                default:
                    break;
            }
        }
    }
    return GWSTATUS_CONTINUE_GAME;
}

//int ticks = (28 – levelNumber) / 4; // levelNumber is the current
// // level number (0, 1, 2, etc.)
//if (ticks < 3)
// ticks = 3; // no RageBot moves more frequently than this

int StudentWorld::move()
{
    
    if (m_Avatar != nullptr){
     
        
        for (int i = 0 ; i < m_actors.size(); i++){
            if (m_actors[i]->alive()){ //if alive, do something!
                m_actors[i]->doSomething();
            }
            if (!m_Avatar->alive()){ //IF SOMETHING CAUSES AVATAR TO DIE
                decLives();
                return GWSTATUS_PLAYER_DIED;
            }
            if (m_completedLvl){ //if level is over,
                playSound(SOUND_FINISHED_LEVEL);
                m_bonus = 1000; //reset bonus
                return GWSTATUS_FINISHED_LEVEL;
            }
        }
        m_Avatar->doSomething(); //move down here because of pea anumation
        for (auto it = m_actors.begin(); it != m_actors.end(); it++){
            if(!(*it)->alive()){
                delete *it;
                m_actors.erase(it);
                it = m_actors.begin();
            }
        }
    }
    if (m_bonus > 0){
        m_bonus--; //decrease bonus every tick if it is not already 0
    }
    setDisplayText(); //set display text

//    setGameStatText("Game will end when you type q");
    
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp(){
    
    for(int i = 0; i < m_actors.size(); i++){ //handle memory leaks
        if(m_actors[i] != nullptr){
            delete m_actors[i];
        }
    }
    m_actors.clear();
    
    delete m_Avatar;
    m_Avatar = nullptr;
}

StudentWorld::~StudentWorld(){
    cleanUp(); //call cleanup bc spec says
}

bool StudentWorld::canMoveTo(double x, double y){
    for (int i = 0; i < m_actors.size() ; i++){
        double aXPos = m_actors[i]->getX();
        double aYPos = m_actors[i]->getY();
        bool actorColocation = m_actors[i]->allowsAgentColocation(); //if actor allows colocation
        bool visible = m_actors[i]->isVisible();
        
        if (!actorColocation && x == aXPos && y == aYPos && visible) return false; //found smth not valid
    }
    return true;
}

bool StudentWorld::playerIsOnSquare(double x, double y) const{
    double avatarX = m_Avatar->getX();
    double avatarY = m_Avatar->getY();
    
    if (x == avatarX && y == avatarY) return true;  //player is on this exact spot
    else return false;
}

bool StudentWorld::emptySquare(double x, double y) const{
    for (int i = 0; i < m_actors.size() ; i++){
        double aXPos = m_actors[i]->getX();
        double aYPos = m_actors[i]->getY();
        
        if (x == aXPos && y == aYPos) return false; //if something was found on this spot return false
    }
    return true;
}

bool StudentWorld::canMarbleMoveTo(double x, double y) const{
    for (int i = 0; i < m_actors.size() ; i++){
        double aXPos = m_actors[i]->getX();
        double aYPos = m_actors[i]->getY();
        bool allowsMarble = m_actors[i]->allowsMarble();
        
        if (!allowsMarble && x == aXPos && y == aYPos) return false; //if this actor does not allow a marble return false
    }
    return true;
}

bool StudentWorld::isMarble(double x, double y){
    for (int i = 0; i < m_actors.size() ; i++){
        double aXPos = m_actors[i]->getX();
        double aYPos = m_actors[i]->getY();
        bool swallowable = m_actors[i]->isSwallowable();
        
        if (swallowable && x == aXPos && y == aYPos){ //if there is a marble at this spot
            return true;
        }
    }
    return false;
}

Actor* StudentWorld::getMarble(double x, double y){
    Actor* result = nullptr;
    for (auto it = m_actors.begin(); it != m_actors.end(); it++){
        if((*it)->getX() == x && (*it)->getY() == y && *(it) != getAvatar() && (*it)->isSwallowable()){ //swallowable -> marble
            result = *it; //return pointer to the marble
        }
    }
    return result;
}


Actor* StudentWorld::getActorAt(double x, double y){ //return pointer to actor
    Actor* result = nullptr;
    for (auto it = m_actors.begin(); it != m_actors.end(); it++){
        if((*it)->getX() == x && (*it)->getY() == y && *(it) != getAvatar()){ //if it is not avatar
            result = *it; //return assignment
        }
    }
    return result;
}

void StudentWorld::setDisplayText(){
    int score = getScore();
    int level = getLevel();
    int bonus = getBonus();
    int lives = getLives();
    int ammo = getAvatar()->getAmmo(); //number of peas
    int health = getAvatar()->getHPPercent(); //hp percent in actor.h
    
    ostringstream oss; //string readup on class website
    oss << "Score: " << setw(7) << setfill('0') << score
    << "  Level: " << setw(2) << setfill('0') << level
    << "  Lives: " << setw(2) << setfill(' ') << lives
    << "  Health: " << setw(3) << setfill(' ') << health
    << "%" << "  Ammo: " << setw(3) << setfill(' ') << ammo
    << "  Bonus: " << setw(4) << setfill(' ') << bonus;
    
    setGameStatText(oss.str()); //set game stat
}

void StudentWorld::addPea(double x, double y, int dir){
//    m_actors.push_back(new Pea(this, x, y, dir));
    m_actors.insert(m_actors.begin(), new Pea(this, x, y, dir)); //put at the beginning because of pea animation
}

bool StudentWorld::overlappedWithDamageableActor(double x, double y){
    for (int i = 0; i < m_actors.size() ; i++){
        double aXPos = m_actors[i]->getX();
        double aYPos = m_actors[i]->getY();
        bool damageable = m_actors[i]->isDamageable();
        
        if (damageable && x == aXPos && y == aYPos){ //if there is a damageable actor here
            return true;
        }
    }
    return false;
}

Actor* StudentWorld::getDamageableActor(double x, double y){
    Actor* result = nullptr;
    for (auto it = m_actors.begin(); it != m_actors.end(); it++){
        if((*it)->getX() == x && (*it)->getY() == y && (*it)->isDamageable()){ //if damanageable acotr, return pointer to it
            result = *it;
        }
    }
    return result;
}

Actor* StudentWorld::getGoodieAt(double x, double y){
    Actor* result = nullptr;
    for (auto it = m_actors.begin(); it != m_actors.end(); it++){
        if((*it)->getX() == x && (*it)->getY() == y && (*it)->pickupable() && (*it)->isVisible()){ //return pointer to goodie at x,y
            result = *it;
        }
    }
    return result;
}

bool StudentWorld::overlappedWithStopsPea(double x, double y){
    if (playerIsOnSquare(x, y)) return true;
    for (int i = 0; i < m_actors.size() ; i++){
        double aXPos = m_actors[i]->getX();
        double aYPos = m_actors[i]->getY();
        bool stopsPea = m_actors[i]->stopsPea();
        
        if (stopsPea && x == aXPos && y == aYPos){ //if it is on top of something that stops pea
            return true;
        }
    }
    return false;
}

bool StudentWorld::playerSameRow(double y){
    if (m_Avatar->getY() == y) return true; //same row
    else return false;
}

bool StudentWorld::playerSameCol(double x){
    if (m_Avatar->getX() == x) return true; //same col
    else return false;
}

bool StudentWorld::canRobotMoveTo(double x, double y){
    if (playerIsOnSquare(x, y)) return false;
    for (int i = 0; i < m_actors.size() ; i++){
        double aXPos = m_actors[i]->getX();
        double aYPos = m_actors[i]->getY();
        bool robotColocation = m_actors[i]->robotColocation();
        bool visible = m_actors[i]->isVisible();
        
        if (!robotColocation && x == aXPos && y == aYPos && visible) return false; //if robot colocation isn't allowed return false
    }
    return true; //nothing invalid found
}


bool StudentWorld::hasClearShot(double x, double y, int dir) {
    double playerX = getAvatar()->getX();
    double playerY = getAvatar()->getY();

    switch (dir) {
        case GraphObject::right:
            if (playerY == y && playerX > x) {
                // iterate from the target position towards the player's position along the row
                for (double checkX = x + 1; checkX < playerX; checkX++) {
                    if (checkX == playerX){ //if player is found first,
                        return true; //return true
                    }
                    Actor* ptr = getActorAt(checkX, y);
                    if (ptr != nullptr){
                        if (!ptr->robotcanShootThrough()) {
                            return false; // obstacle found so no clear shot
                        }
                    }
                }
            }
            else{
                return false;
            }
            break;
        case GraphObject::left:
            if (playerY == y && playerX < x) {
                // iterate from the target position towards the player's position along the row
                for (double checkX = x - 1; checkX > playerX; checkX--) {
                    if (checkX == playerX){ //player is found first
                        return true;
                    }
                    Actor* ptr = getActorAt(checkX, y);
                    if (ptr != nullptr){
                        if (!ptr->robotcanShootThrough()) {
                            return false; // obstacle found so no clear shot
                        }
                    }
                }
            }
            else{
                return false;
            }
            break;
        case GraphObject::up:
            if (playerX == x && playerY > y) {
                // same thing, iterate
                for (double checkY = y + 1; checkY < playerY; checkY++) {
                    if (checkY == playerY){ //player is found first
                        return true;
                    }
                    Actor* ptr = getActorAt(x, checkY);
                    if (ptr != nullptr){
                        if (!ptr->robotcanShootThrough()) {
                            return false; //something is blcoking
                        }
                    }
                }
            }
            else{
                return false;
            }
            break;
        case GraphObject::down:
            if (playerX == x && playerY < y) {
                // same thing iterating downwards
                for (double checkY = y - 1; checkY > playerY; checkY--) {
                    if (checkY == playerY){ //player is found first
                        return true;
                    }
                    Actor* ptr = getActorAt(x, checkY);
                    if (ptr != nullptr){
                        if (!ptr->robotcanShootThrough()) {
                            return false; // something is blocking
                        }
                    }
                }
            }
            else{
                return false;
            }
            break;
        default:
            return false;
    }
    
    // No obstacles found, clear shot
    return true;
}


bool StudentWorld::thiefBotHere(double x, double y){
    for (int i = 0; i < m_actors.size() ; i++){
        double aXPos = m_actors[i]->getX();
        double aYPos = m_actors[i]->getY();
        bool thiefBot = m_actors[i]->isThiefBot();
        
        if (thiefBot && x == aXPos && y == aYPos) return true; //if there is a thiefbot found here
    }
    return false;
}


//int StudentWorld::thiefBotCensus(double x, double y){
//    int count = 0;
//    for (int i = y+3; i >= y-3; i--){
//        for (int j = x-3; j <= x + 3; j++){
//            if (thiefBotHere(i, j)){
//                    count++;
//            }
//        }
//    }
//    return count;
//}

int StudentWorld::thiefBotCensus(double x, double y, int dist){
    int count = 0;
    for (int i = 0; i < m_actors.size(); i++){
        if (m_actors[i]->isThiefBot()){
            if (m_actors[i]->getX() >= x - dist && m_actors[i]->getX() <= x + dist && m_actors[i]->getY() >= y - dist && m_actors[i]->getY() <= y + dist){
                count++; //every time it finds a thiefbot within the radius
            }
        }
    }
    return count;
}

void StudentWorld::addRegularThiefBot(double x, double y, int dir){
    m_actors.push_back(new RegularThiefBot(this, x, y, dir));
}

void StudentWorld::addMeanThiefBot(double x, double y, int dir){
    m_actors.push_back(new MeanThiefBot(this, x, y, dir));
}

bool StudentWorld::goodieOnSquare(double x, double y){
    for (int i = 0; i < m_actors.size() ; i++){
        double aXPos = m_actors[i]->getX();
        double aYPos = m_actors[i]->getY();
        bool goodie = m_actors[i]->pickupable();
        
        if (goodie && x == aXPos && y == aYPos){ //if there is a goodie found on this square
            return true;
        }
    }
    return false;
}



bool StudentWorld::canThiefBotMoveTo(double x, double y, double dx, double dy) const{
    if(playerIsOnSquare(x+dx, y+dy)) return false;
    for (int i = 0; i < m_actors.size() ; i++){
        double aXPos = m_actors[i]->getX();
        double aYPos = m_actors[i]->getY();
        bool robotColocation = m_actors[i]->robotColocation();
        bool visible = m_actors[i]->isVisible();
        
        if (!robotColocation && x+dx == aXPos && y+dy == aYPos && visible) return false; //if the thiefbot cannot move here beause of robot colocation restriction
    }
    return true;
}
