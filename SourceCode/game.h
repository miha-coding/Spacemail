#ifndef GAME_H
#define GAME_H

#include <QMainWindow>
#include "simulation.h"
#include "planet.h"
#include "spacecraft.h"
#include "painter.h"
#include "player.h"
#include "setting.h"

#include<QGroupBox>
#include<QVector>
#include<QPixmap>
#include<QPushButton>
#include<QLabel>
#include<QLineEdit>
#include<QGridLayout>
#include<QMap>
#include<QMenu>
#include<QMutex>
#include<QSemaphore>
#include<memory>

#include<QDebug>


class Game : public QMainWindow
{
    Q_OBJECT
private:

    double gamma; //gravitational constant
    double pi2; //2*pi
    double maxSimulationTime;
    int numOfPlanets;
    int maxPlSize,minPlSize;  // Planet size

    std::shared_ptr<QVector<Player>> player;
    std::shared_ptr<QStringList> playerNames, playerColor;
    int curPlayer; //Position of current Player in list player

    std::unique_ptr<Simulation> sim;
    std::shared_ptr<QVector<Planet>> planetList;
    std::unique_ptr<Painter> paint;
    std::shared_ptr<Spacecraft> sc;
    std::shared_ptr<QSemaphore> mutex;
    std::shared_ptr<Vector2d> drawPointA;  // Two points for
    std::shared_ptr<Vector2d> drawPointB;  // plotline.
    bool simRunning;
    int gameMode;  // 1: OnePlanet, 2: OtherPlanet, 3: AllPlanets

    QPixmap pic;
    QPixmap copy_pic;
    int picWidth,picHeight;
    Vector2d zero;  // Origin
    QLabel *vLab,*aLab,*imgLab,*nameLab,*name;
    QLineEdit *vLine, *aLine;
    QPushButton *goBut;
    QGridLayout *lay;
    QGroupBox *centralGroup;

    QMenu *menu,*playerMenu;

    //Needed for playerSettings
    std::unique_ptr<Setting> settings;
    QLineEdit *colLine,*nameLine;

    // Data for game modes
    std::unique_ptr<QMap<int, QList<int>*>> allPlanet;
    std::unique_ptr<QMap<int, int>> onePlanet, otherPlanet;

    void createMenu();
public:
    Game(QWidget *parent = 0);
    double px2m(double p) { return p*200000; }
    double m2px(double m) { return m/200000; }
    void loadStdPlayer();
    void nextPlayer();
    void resizePlayerList(int n);
    void goToPlayer(int i);
    void initGame();
    void checkForWin();

public slots:
    void startSimulation();
    void updatePix();
    void visualize_direction(const QString& str);

    //Menu slots
    void renewPic();
    void goToPlayer();
    void newGame();
    void save();
    void load();
    void saveStdPic();
    void loadStdPic();
};

#endif // GAME_H
