//
// Created by mkowa on 27.08.2024.
//

#include <raylib.h>
#include <unordered_map>
#include <queue>
#include "coinflip.h"
#include "timer.h"
#include "display.h"
#include <thread>
#include <unordered_set>
#include <utility>
#include <vector>
#include <iostream>
#include <fstream>

#include "ecs.h"
//
// Created by mkowa on 27.08.2024.
//



unsigned int ecs::fire_system::counter=0;

ecs::position_component::position_component(int x, int y, char w, Color a) : posx(x), posy(y), what(w), col(a) {};
ecs::life_component::life_component(bool state) : cellstate(state) {}

ecs::spreadable_component::spreadable_component(std::unordered_set<char> spread, float interval) :

dspread_timer(timer::Timer(std::chrono::milliseconds((int) (interval * 1000)))), spreads_to(std::move(spread)) {}

ecs::fire_component::fire_component(std::unordered_set<char> spread, float interval, float btimer, int chanced) :
spreadable_component(std::move(spread), interval),
burnout_timer(timer::Timer(std::chrono::milliseconds((((int) (btimer * 1000)))))), chance(chanced) {}

template<class T1, class T2>
unsigned long long int ecs::PairHash::operator()(const std::pair<T1, T2> &v) const {
    return std::hash<T1>()(v.first) ^ std::hash<T2>()(v.second) << 1;
}

void ecs::spreading_system::update(ecs::registry &r_reg) {
    for (unsigned long long int e = 1; e <= r_reg.max_entity; e++) {
        if (r_reg.spreadables.contains(e)) {
            //if(r_reg.spreadables[e].spread_timer>0) r_reg.spreadables[e].spread_timer--;
            if (r_reg.spreadables.at(e).dspread_timer.isFinished()) {

                //find neighbours to spread to
                // Bad! Iterates through all entities!
                for (auto &[key, value]: r_reg.displayable_positions) {
                    if (r_reg.spreadables.at(e).spreads_to.contains(value.what) &&
                    areNeighbours(r_reg, e, key)) {
                        if (roll::flipCoin()) spread(r_reg, e, key);
                        }
                }
            }
        }
    }
}
void ecs::spreading_system::spread(ecs::registry &r_reg, unsigned long long int e, unsigned long long int b) {
            r_reg.spreadables.at(e).dspread_timer.reset();
            r_reg.displayable_positions.at(b).what = r_reg.displayable_positions.at(e).what;
            r_reg.spreadables.emplace(b, r_reg.spreadables.at(e));
}
// Obiekt nie sąsiaduje z innymi obiektami na jego polu
bool ecs::spreading_system::areNeighbours(ecs::registry &r_reg, unsigned long long int a, unsigned long long int b) {
    if (a == b) return false;
    if (r_reg.displayable_positions.contains(a) && r_reg.displayable_positions.contains(b)) {
        int aposx = r_reg.displayable_positions.at(a).posx;
        int aposy = r_reg.displayable_positions.at(a).posy;
        int bposx = r_reg.displayable_positions.at(b).posx;
        int bposy = r_reg.displayable_positions.at(b).posy;
        if (aposx - bposx >= -1 and aposx - bposx <= 1 and aposy - bposy >= -1 and aposy - bposy <= 1) {
            return true;
        }
    }
    return false;
}

std::vector<unsigned long long int> ecs::spreading_system::getNeighbours(ecs::registry &r_reg, unsigned long long int a) {
    std::vector<unsigned long long int> neighbours;
    if (r_reg.displayable_positions.contains(a)){
        int x = r_reg.displayable_positions.at(a).posx;
        int y = r_reg.displayable_positions.at(a).posy;
        for (int px = -1; px <= 1; px++) {
            for (int py = -1; py <= 1; py++) {
                if (r_reg.entitypos.contains({x + px, y + py}) and !(px == 0 && py == 0))
                    neighbours.push_back(r_reg.entitypos.at({x + px, y + py}));
            }
        }
    }
    return neighbours;
}

void ecs::fire_system::update(ecs::registry &r_reg, display::Screen &screen) {
    bool cont = true;
    while (!firequeue.empty()&&cont) {
        unsigned long long int q_element = firequeue.front();
        if (r_reg.fires.contains(q_element)) {
            if(r_reg.fires.at(q_element).burnout_timer.isFinished())
            {
                burnout(q_element, r_reg, screen);
                firequeue.pop();
            }
            else { cont = false; };
        }
        else firequeue.pop();
    }
    Color c;
    for (auto &[e, v]: r_reg.fires) {
        v.burnout_timer.isFinished();
        auto t = v.burnout_timer.getElapsed();
        auto r = v.burnout_timer.duration;
        float a = 1 - t/r;
        if(a<0.2) a= 0.2;
        //else if(a>0.8) c = Color(253*a,249*a,0, 255);
         c = display::colorGradient(a,BLACK,RED,YELLOW);
                 //Color(230*a,41*a,55*a, 255);
        if(r_reg.displayable_positions.contains(e)) r_reg.displayable_positions.at(e).col = c;
        if (v.dspread_timer.isFinished()) {
            for (auto a: getNeighbours(r_reg, e)) {
                if (r_reg.displayable_positions.contains(a)) {
                    if (v.spreads_to.contains(r_reg.displayable_positions.at(a).what)) {
                        if (roll::percentileRoll(v.chance)) spread(r_reg, e, a);
                    }
                }
            }

        }
    }

}

void ecs::fire_system::burnout(unsigned long long int e, ecs::registry &r_reg, display::Screen &screen) {

    if (r_reg.displayable_positions.contains(e)) {
        if (r_reg.entitypos.contains({r_reg.displayable_positions.at(e).posx, r_reg.displayable_positions.at(e).posy})) {
            r_reg.entitypos.erase({r_reg.displayable_positions.at(e).posx, r_reg.displayable_positions.at(e).posy});
            screen.removeFromBuffer(r_reg.displayable_positions.at(e).posx, r_reg.displayable_positions.at(e).posy);
            r_reg.displayable_positions.erase(e);
        }
    }
    if(r_reg.fires.contains(e)) r_reg.fires.erase(e);
}

void ecs::fire_system::spread(ecs::registry &r_reg, unsigned long long int e, unsigned long long int b) {
    counter++;
    std::cout<<"Counter : "<<counter<<'\n';
    if (r_reg.fires.contains(b)) return;
    if(!(r_reg.fires.contains(e)||r_reg.displayable_positions.contains(e)||r_reg.displayable_positions.contains(b))) return;
    r_reg.fires.at(e).dspread_timer.reset();
    r_reg.displayable_positions.at(b).what = r_reg.displayable_positions.at(e).what;
   // r_reg.displayable_positions.at(b).col = RED;
    r_reg.fires.emplace(b, r_reg.fires.at(e));
    r_reg.fires.at(b).burnout_timer.reset();
    r_reg.entitypos.at({r_reg.displayable_positions.at(b).posx, r_reg.displayable_positions.at(b).posy}) = b;
    firequeue.push(b);
}
void ecs::display_system::update(ecs::registry &r_reg, display::Screen &screen) {
    for (auto &[e, r]: r_reg.displayable_positions) {
        screen.writeToBuffer(r.col,
                             r.posx,
                             r.posy);
        r_reg.entitypos[{r.posx, r.posy}] = e;
    }
}

void ecs::display_system::render(display::Screen &screen) {
    while (screen.isActive()) {
        screen.renderFrame();
    }
}

void ecs::life_system::update(ecs::registry &r_reg,bool forced) {
    if(!forced&&(m_is_paused || !m_steptimer.isFinished())) return;
    m_steptimer.duration=std::chrono::milliseconds((((int) (m_steptimerdur * 1000))));
    m_steptimer.reset();
    for (auto &[e, r]: r_reg.lives) {
        std::queue<unsigned long long int> q = getLiveNeighbours(r_reg, e);
        if (!r_reg.lives.at(e).cellstate) {
            if (q.size() == 3) switchq.push(e);
        }
        else {
            if (q.size() < 2 || q.size() > 3) switchq.push(e);
        }
    }
    for (; !switchq.empty(); switchq.pop()) {
        switchState(switchq.front(), r_reg);
    }
}
void ecs::life_system::switchState(unsigned long long int &e, ecs::registry &r_reg) {
    if (r_reg.lives.at(e).cellstate) {
        r_reg.lives.at(e).cellstate = false;
        r_reg.displayable_positions.at(e).what = (char) 178;
        r_reg.displayable_positions.at(e).col = Color(0, 0, 0, 255);
    } else {
        r_reg.lives.at(e).cellstate = true;
        r_reg.displayable_positions.at(e).what = (char) 176;
        r_reg.displayable_positions.at(e).col = Color(255, 255, 255, 255);
    }
}

std::queue<unsigned long long int> ecs::life_system::getLiveNeighbours(ecs::registry &r_reg, unsigned long long int a) {
    std::queue<unsigned long long int> neighbours;
    int x = r_reg.displayable_positions.at(a).posx;
    int y = r_reg.displayable_positions.at(a).posy;
    for (int px = -1; px <= 1; px++) {
        for (int py = -1; py <= 1; py++) {
            if (r_reg.entitypos.contains({x + px, y + py}) and !(px == 0 && py == 0) and
            r_reg.lives.at(r_reg.entitypos.at({x + px, y + py})).cellstate == 1)
                neighbours.push(r_reg.entitypos.at({x + px, y + py}));
        }
    }
    return neighbours;
}

bool ecs::life_system::saveState(ecs::registry &r_reg,std::string filename, int dx, int dy) {
    std::ofstream out;
    out.open(filename);
if(out.is_open())
    {
        out<<dx<<" "<<dy<<std::endl;
         for (auto &[e, r]: r_reg.lives) {
            if (r_reg.lives.at(e).cellstate) {
                if(r_reg.displayable_positions.contains(e))
                {
                    out<<r_reg.displayable_positions.at(e).posx<<" "<<r_reg.displayable_positions.at(e).posy<<std::endl;
                }
            }
        }
         out.close();
         return true;
    }
return false;
}

ecs::lifevars ecs::life_system::loadState(std::string filename)
{
    lifevars vars;
    std::string line;
    int x,y;
    std::ifstream in;
    in.open(filename);
if(in.is_open())
{
    in>>vars.dx>>vars.dy;
    while(in>>x>>y)
    {
        vars.alives.emplace_back(x,y);
    }
    in.close();
    vars.ok = true;
    return vars;
}
vars.ok = false;
return vars;
}

void ecs::life_system::pause() {
    m_is_paused=true;
}

void ecs::life_system::unpause() {
    m_is_paused=false;
    m_steptimer.duration=std::chrono::milliseconds((((int) (m_steptimerdur * 1000))));
m_steptimer.reset();

}

ecs::life_system::life_system(float dur) : m_steptimerdur(dur), m_steptimer(timer::Timer((std::chrono::milliseconds((((int) (dur * 1000))))))){
m_steptimer.reset();
}

bool ecs::life_system::isPaused() {
    return m_is_paused;
}

void ecs::life_system::changeDur(float dur) {
m_steptimerdur=dur;
m_steptimer.reset();
}

float ecs::life_system::getDur() {
return m_steptimerdur;
}

unsigned long long int ecs::Game::createEntity() {
    static unsigned long long int entities = 0;
    ++entities;
    m_registry.max_entity = entities;
    return entities;
}

void ecs::Game::removeEntity(unsigned long long int entity) {
    if (m_registry.displayable_positions.contains(entity)) {
        if (m_registry.entitypos.contains({m_registry.displayable_positions.at(entity).posx,m_registry.displayable_positions.at(entity).posy})) {
            m_registry.entitypos.erase({m_registry.displayable_positions.at(entity).posx,m_registry.displayable_positions.at(entity).posy});
        }
        m_registry.displayable_positions.erase(entity);
    }
    if (m_registry.lives.contains(entity)) {
        m_registry.lives.erase(entity);
    }
    if (m_registry.fires.contains(entity)) {
        m_registry.fires.erase(entity);
    }
    if (m_registry.spreadables.contains(entity)) {
        m_registry.spreadables.erase(entity);
    }
}
bool ecs::Game::isRunning() const {
    return m_is_running;
}

ecs::registry &ecs::Game::getRegistry() {
    return m_registry;
}

void ecs::Game::update(display::Screen &screen) {
    // m_system.update(m_registry)
    fire_system.update(m_registry, screen);
    m_display_system.update(m_registry, screen);
    m_spreading_system.update(m_registry);
    game_life_system.update(m_registry);
}
void ecs::Game::startDisplay(display::Screen &screen) {
    screen.start();
    render_thread = std::thread([a = &m_display_system, this, &screen]() {
        a->render(screen);
    });
    m_is_rendering=true;
}
void ecs::Game::stopDisplay(display::Screen &screen) {
    m_is_rendering=false;
    screen.stop();
    render_thread.join();
}
void ecs::Game::stop() {
    m_is_running = false;
}

ecs::Game::Game(const display::Screen &Scr) {
    m_display_system.dx = Scr.getdx();
    m_display_system.dy = Scr.getdy();
    m_is_running = true;
    m_is_rendering = false;
    m_registry.displayable_positions.reserve( Scr.getdx() * Scr.getdy());
    m_registry.lives.reserve( Scr.getdx() * Scr.getdy());
    m_registry.fires.reserve( Scr.getdx() * Scr.getdy());
    m_registry.entitypos.reserve( Scr.getdx() * Scr.getdy());
}
std::ostream &ecs::operator<<(std::ostream &F, const ecs::lifevars &w) {
    F << w.dx << " " << w.dy << "\n";
    for(auto a : w.alives)
    {
        F<<a.first<<" "<<a.second<<'\n';
    }
    return F;
}

std::ostream &ecs::operator<<(std::ostream &F, const ecs::firevars &w) {
    return F << w.dx << " " << w.dy<< "\n"<<"Burn time: "<<w.fire_burn_time<< "\n"<< "Spread int:" << w.fire_spread_time<< "\n" << "Spread chance: "<<w.fire_spread_chance<< "\n" << " Forest: "<<w.forest<< "\n";
}