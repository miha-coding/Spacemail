#include "game.h"
#include<QDebug>
#include<QDialog>
#include "vektor.h"
#include<QPicture>
#include<QPainter>
#include<QFile>
#include<stdlib.h>
#include<QStatusBar>
#include<QMenuBar>
#include<QLabel>
#include<QMenu>
#include<QPushButton>
#include<QSpinBox>
#include<QDoubleSpinBox>
#include<QDialogButtonBox>
#include<QComboBox>
#include<QMessageBox>
#include<QFileDialog>
#include<QTableWidget>
#include<QRandomGenerator>
#include<QSettings>
#include<QBuffer>
#include<QVector>

#include "setting.h"

Game::Game(QWidget *parent)
    : QMainWindow(parent)
{
    srand(time(NULL));
    // constants for simulation
    gamma = 6.673848*pow(10, -11);  // gravitational constant
    pi2 = 6.2831853071795;  // 2*pi

    // Drawing area
    picWidth = 800;
    picHeight = 600;
    pic = QPixmap(picWidth,picHeight);
    // Set coordinates of the origin
    zero = Vector2d(picWidth/2, picHeight/2);
    // Theoretically there is no max Sim time needed (it can be stopped by
    // means of a button or stops if a collision occurs).
    // No max available?! Use max(a,b) = -min(-a,-b) instead
    maxSimulationTime = 600 * -std::min(-picHeight,-picWidth);
    // Min and max planet radius
    maxPlSize = 50;
    minPlSize = 5;


    // Player and Colors from file std.txt
    playerNames = std::make_shared<QStringList>();  // Name of all player
    playerColor = std::make_shared<QStringList>();  // Color to each player
    player = std::make_shared<QVector<Player>>();
    loadStdPlayer();
    this->curPlayer = 0;
    gameMode = 1;

    sc = std::make_shared<Spacecraft>();
    planetList = std::make_shared<QVector<Planet>>();
    mutex = std::make_shared<QSemaphore>();

    // Create planets
    numOfPlanets = 5;
    settings = std::make_unique<Setting>(player, playerNames, playerColor, planetList,
                                         minPlSize, maxPlSize, picWidth, picHeight,
                                         numOfPlanets, this);
    settings.get()->fillPlanetList(numOfPlanets);

    drawPointA = std::make_shared<Vector2d>();
    drawPointB = std::make_shared<Vector2d>();

    sim = std::make_unique<Simulation>(sc, planetList, mutex, drawPointA, drawPointB);
    sim.get()->setpicHeight(picHeight);
    sim.get()->setpicWidth(picWidth);

    // If simulation step is calculated plot the current trajectory
    connect(sim.get(),SIGNAL(finished()),this,SLOT(updatePix()));

    paint = std::make_unique<Painter>(sc, planetList, &pic, mutex, imgLab, drawPointA, drawPointB);
    paint.get()->setZero(zero);
    paint.get()->plotPlanets();
    this->saveStdPic();

    connect(paint.get(), SIGNAL(paintFinished()), this, SLOT(updatePix()));
    connect(sim.get(), SIGNAL(collision()), this, SLOT(startSimulation()));

    // Design of the main window
    //pic.fill();
    imgLab = new QLabel();
    nameLab = new QLabel(tr("Name:"));
    name = new QLabel;

    vLab = new QLabel(tr("Velocity:"));
    aLab = new QLabel(tr("km/s\t Angle"));
    vLine = new QLineEdit("100");
    aLine = new QLineEdit("90");
    vLine->setInputMask("");
    //Add Labels/LineEdits for units
    goBut = new QPushButton(tr("Go!"));


    imgLab->setPixmap(pic);
    lay = new QGridLayout();
    lay->addWidget(imgLab,3,1,5,11,Qt::AlignLeft);
    lay->addWidget(nameLab,2,1,1,1,Qt::AlignLeft);
    lay->addWidget(name,2,2,1,1,Qt::AlignLeft);
    lay->addWidget(vLab,2,3,1,1,Qt::AlignLeft);
    lay->addWidget(vLine,2,4,1,2,Qt::AlignLeft);
    lay->addWidget(aLab,2,6,1,1,Qt::AlignLeft);
    lay->addWidget(aLine,2,7,1,2,Qt::AlignLeft);
    lay->addWidget(goBut,2,9,1,1,Qt::AlignLeft);
    //lay->setMargin(10);
    lay->setHorizontalSpacing(2);

    centralGroup = new QGroupBox();
    centralGroup->setLayout(lay);
    this->setCentralWidget(centralGroup);

    connect(aLine, SIGNAL(textChanged(QString)), this, SLOT(visualize_direction(QString)));
    connect(goBut,SIGNAL(clicked(bool)),this,SLOT(startSimulation()));

    simRunning = 0;

    onePlanet = std::make_unique<QMap<int, int>>();
    otherPlanet = std::make_unique<QMap<int, int>>();
    allPlanet = std::make_unique<QMap<int, QList<int>*>>();

    statusBar();
    menuBar();
    createMenu();
    initGame();
    nextPlayer();
    visualize_direction(vLine->text());

}

void Game::loadStdPlayer() {
    /* Load std player from the file std.txt.
     * If there is no entry in the file set a std player.
     */

    QFile f(QDir::currentPath().append("/std.txt"));
    f.open(QFile::ReadOnly);
    QTextStream str(&f);
    int num=1;
    // In every line in file there is maximal one player with a color
    while(!str.atEnd()) {
        QStringList read = str.readLine().split(";");
        if(read.size()>=2 && read.at(0) != "") {
            playerNames.get()->append(read.at(0));
        }
        else {
            playerNames.get()->append(tr("Player %1").arg(num));
            num++;
        }
        if(read.size()>=2 && read.at(1) != "") {
            playerColor.get()->append(read.at(1));
        }
        else {
            // Choose a random color
            int r = rand();
            int g = rand();
            int b = rand();
            playerColor.get()->append(QColor(r,g,b).name());
        }

    }
    f.close();

    // Set a player if there is no player in std.txt.
    QString name1 = tr("Player 1");
    QColor r(Qt::red);
    if(!playerNames.get()->isEmpty()) name1 = playerNames.get()->at(0);
    if(!playerColor.get()->isEmpty()) r = QColor(playerColor.get()->at(0));

    //Create new player and add it to the player list.
    Player p(name1,r,0,0);
    player.get()->append(p);
}

void Game::startSimulation() {
    /* Starts and stops the simulation. */

    // If the simulation is running stop it.
    if(simRunning) {
        qDebug() << "Game::startSimulation: Stop the simulation";
        sim.get()->quit();
        sim.get()->setKeepRunning(0);
        sim.get()->wait();
        paint.get()->quit();
        paint.get()->setKeepRunning(0);
        paint.get()->wait();

        int i=1;
        while(sim.get()->isRunning()) { qDebug() << "Game::startSim: sim running" << sim.get()->getKR() << i;i+=1; }
        while(paint.get()->isRunning()) { qDebug() << "Game::startSim: paint running" << i;i+=1; }
        simRunning = 0;
        this->goBut->setText(tr("Go"));
        updatePix();
        bool ok1,ok2;
        // Qt thinks player is a list so I use operator[]
        player.get()->operator[](curPlayer).setLastA(aLine->text().toDouble(&ok1));
        player.get()->operator[](curPlayer).setLastV(vLine->text().toDouble(&ok2));
        if(!ok1) player.get()->operator[](curPlayer).setLastA(0);
        if(!ok2) player.get()->operator[](curPlayer).setLastV(0);

        checkForWin();
        nextPlayer();
        return;
    }

    // Start the simulation.

    // Set velocity and angle.
    bool ok1,ok2;
    double v = vLine->text().toDouble(&ok1);
    double alpha = aLine->text().toDouble(&ok2);
    if(!ok1 || !ok2) {
        QMessageBox::critical(nullptr, tr("Start Simulation"),
                              tr("Velocity or angle input is not valid!"));
        qDebug() << "Game::startSim: Velocity or angle input is not valid!";
        return;
    }

    simRunning = 1;
    this->goBut->setText(tr("Stop"));  // Change button text.

    mutex.get()->acquire(mutex.get()->available()); //Semaphore auf 0 setzen

    // Set simulation attributes, initial values and start the simulation.
    sim.get()->setKeepRunning(1);
    sim.get()->setV(v);
    sim.get()->setAlpha(alpha);
    sim.get()->start();

    paint.get()->setKeepRunning(1);
    paint.get()->start();


}

void Game::nextPlayer() {
    /* Fills window elements with information about the next player. */

    //qDebug() << "Game::nextPlayer: " << curPlayer << " " << player->size();
    curPlayer = (curPlayer+1) % player.get()->size();

    name->setText(player.get()->at(curPlayer).getName());
    vLine->setText(QVariant(player.get()->at(curPlayer).getLastVelocity()).toString());
    aLine->setText(QVariant(player.get()->at(curPlayer).getLastAngle()).toString());
    QColor playerCol = player.get()->at(curPlayer).getCol();
    paint.get()->setColor(playerCol);
    paint.get()->plotPlanets(); // Reset Color of Planets

    //switch game Mode
    switch(gameMode) {
    case 1: // OnePlanet
        paint.get()->plotPlanet(planetList.get()->at(onePlanet.get()->value(curPlayer)), playerCol);
        break;
    case 2: // OtherPlanet
        paint.get()->plotPlanet(planetList.get()->at(otherPlanet.get()->value(curPlayer)), playerCol);
        break;
    case 3:
        for(int i=0; i<allPlanet.get()->value(curPlayer)->size(); ++i) {
            paint.get()->plotPlanet(planetList.get()->at(allPlanet.get()->value(curPlayer)->at(i)),
                                    playerCol);
        }
        break;
    default:
        qDebug("Game::nextPlayer: Spiel läuft nicht, da ein Spieler schon gewonnen hat."
               "Bitte neues Spiel starten!");
    }
    this->imgLab->setPixmap(pic);
    //qDebug() << "nextPlayer: Picture set.";
    visualize_direction(aLine->text());

}

void Game::updatePix() {
    /* Sets the current picture and updates the status bar. */

    //Set picture
    if(!sim.get()->isRunning()) return;

    this->imgLab->setPixmap(pic);

    // Get and set velocity
    Vector2d tmp = sc.get()->getVelocity();
    QString txt = "";
    txt.append(tr("v: %1 km/s,\t").arg(QVariant(sqrt(pow(tmp(0),2)+pow(tmp(1),2))/1000).toFloat()));

    // Get and set position
    tmp = sc.get()->getPosition();
    txt.append(tr("x: %1 km (%2 Px),\t").arg(QVariant(tmp(0)).toInt()).arg(QVariant(m2px(tmp(0))).toInt()));
    txt.append(tr("y: %1 km (%2 Px)\t").arg(QVariant(tmp(1)).toInt()).arg(QVariant(m2px(tmp(1))).toInt()));

    statusBar()->showMessage(txt); //in statusBar;

}

void Game::resizePlayerList(int n) {
    /* Updates the player list to a new number of player n (int). */

    // Old size was larger -> remove player
    while(player.get()->size()>n) {
        //Delete last player as long as #Player > n
        player.get()->removeLast();
    }

    // Old size was lower -> add player
    while(player.get()->size()<n) {
        //Append Player as long as #Player < n
        int r = rand();
        int g = rand();
        int b = rand();
        QString plName = tr("Player %1").arg(QVariant(player.get()->size()+1).toString());
        if(player.get()->size() < playerNames.get()->size()) {
            plName = playerNames.get()->at(player.get()->size());
        }
        Player p(plName,QColor(r,g,b),0,0);
        player.get()->append(p);
    }
    initGame();
    this->nextPlayer();  // Set the next player as current player. This is
                         // important if player are removed since the current
                         // player could be a removed player.


}

void Game::newGame() {
    // Adjust player settings (name, color) for all player (menu new Setting)

    int res = settings.get()->exec();
    if(res == QDialog::Accepted) {
        // Set the new settings.
        settings.get()->applySetting();

        this->gameMode = settings.get()->getGameMode();
        picHeight = settings.get()->getPicHeight();
        picWidth = settings.get()->getPicWidth();
        pic = QPixmap(picWidth, picHeight);
        zero = Vector2d(picWidth/2, picHeight/2);
        paint.get()->setZero(zero);
        this->renewPic();
        // QRandomGenerator bounded: Highest exclusive => 0..n-1
        curPlayer = QRandomGenerator::global()->bounded(0, this->player.get()->count());
        initGame();
        this->nextPlayer();
        vLine->setText("0");
        aLine->setText("0");

        vLine->setEnabled(true);
        aLine->setEnabled(true);
        goBut->setEnabled(true);

        visualize_direction(vLine->text());
    }

}

void Game::createMenu() {
    /* Creates the menu of the main window. */

    menu = new QMenu(tr("Game"));
    menu->addAction(tr("New Game"),this,SLOT(newGame()));
    menu->addSeparator();
    menu->addAction(tr("Save Trajectories"),this,SLOT(saveStdPic()));
    menu->addAction(tr("Delete new Trajectories"),this,SLOT(loadStdPic()));
    menu->addSeparator();
    menu->addAction(tr("Save"),this,SLOT(save()));
    menu->addAction(tr("Load"),this,SLOT(load()));
    menu->addSeparator();
    menu->addAction(tr("Close"),this,SLOT(close()));
    menuBar()->addMenu(menu);

    playerMenu = new QMenu(tr("Player"));
    playerMenu->addAction(tr("GoTo Player"),this,SLOT(goToPlayer()));
    menuBar()->addMenu(playerMenu);

}

void Game::goToPlayer() {
    QDialog *dial = new QDialog();
    QLabel *numLab  = new QLabel(tr("Go to Player:"),dial);
    QComboBox *plComb = new QComboBox(dial);
    QPushButton *ok = new QPushButton(tr("Ok"),dial);
    QPushButton *cancel = new QPushButton(tr("Cancel"),dial);
    QDialogButtonBox *dialBox = new QDialogButtonBox(dial);
    QGridLayout *dialLay = new QGridLayout(dial);

    for(int i =0;i<player.get()->size();i++) {
        plComb->addItem(player.get()->takeAt(i).getName());
    }
    plComb->setCurrentIndex(curPlayer);

    dialBox->addButton(ok,QDialogButtonBox::AcceptRole);
    dialBox->addButton(cancel,QDialogButtonBox::RejectRole);

    connect(ok,SIGNAL(clicked(bool)),dial,SLOT(accept()));
    connect(cancel,SIGNAL(clicked(bool)),dial,SLOT(reject()));

    dialLay->addWidget(numLab,1,1,1,1);
    dialLay->addWidget(plComb,1,2,1,1);
    dialLay->addWidget(dialBox,2,1,1,2);

    int res = dial->exec();

    if(res == QDialog::Rejected) return;

    int index = plComb->currentIndex();
    goToPlayer(index);

    delete dial;
}

void Game::goToPlayer(int i) {
    /* Continue the game with player i. */
    if(i>player.get()->size()) return;
    curPlayer = i-1;  // Goto player 1 (index i = 0) -> player.at(0).
                      // So set curPlayer = i-1 -> nextPlayer: curPlayer = 0;
    nextPlayer();
}

void Game::save() {
    /* Saves the last player velocity and angle and all planets to a file. */

    QString dir = QDir::currentPath().append("/data");
    // Let the user choose a filename
    QString f = QFileDialog::getSaveFileName(0,tr("Save"),dir,"*.SAV");
    if(f=="") return;
    //pic.save(f,"PNG");
    if(!f.endsWith(".SAV"))
        f.append(".SAV");

    QSettings setting(f, QSettings::IniFormat);

    // GameMode
    setting.setValue("GameMode", gameMode);
    // Save the pic
    QByteArray bArr;
    QBuffer buffer(&bArr);
    buffer.open(QIODevice::WriteOnly);
    pic.save(&buffer, "PNG");
    setting.setValue("pic", bArr);

    // Save the player
    setting.setValue("curPlayer", curPlayer);

    setting.beginGroup("Players");
    for(int i=0;i<player.get()->size();i++) {
        setting.beginGroup(tr("Player %1").arg(i));
        setting.setValue("Name", player.get()->at(i).getName());
        setting.setValue("Color", player.get()->at(i).getCol().name());
        setting.setValue("Velocity", player.get()->at(i).getLastVelocity());
        setting.setValue("Angle", player.get()->at(i).getLastAngle());
        // Target Planets
        setting.beginGroup("Targets");
        switch(gameMode) {
        case 1:
            setting.setValue("planet", onePlanet.get()->value(i));
            break;
        case 2:
            setting.setValue("planet", otherPlanet.get()->value(i));
            break;
        case 3:
            QList<int> *targets = allPlanet.get()->value(i);
            for(int j=0; j<targets->size(); ++j) {
                setting.setValue(QString("Target %1").arg(j), targets->at(i));
            }
        }
        setting.endGroup(); // Targets
        setting.endGroup(); // Player i
    }
    setting.endGroup(); // Players

    // Save the planets
    Vector2d tmp;
    setting.beginGroup("Planets");
    for(int i=0;i<planetList.get()->size();i++) {
        setting.beginGroup(tr("Planet %1").arg(i));
        tmp = planetList.get()->operator[](i).getPosition();
        setting.setValue("x", tmp(0));
        setting.setValue("y", tmp(1));
        setting.setValue("radius", planetList.get()->operator[](i).getRadius());
        setting.endGroup();
    }
    setting.endGroup();

}

void Game::load() {
    /* Load the last player velocity and angle and all planets to a file. */

    QString dir = QDir::currentPath().append("/data");
    // Let the user select a file
    QString f = QFileDialog::getOpenFileName(0,tr("Load"),dir,"*.SAV");
    if(f=="") return;

    QSettings setting(f, QSettings::IniFormat);

    QByteArray bArr = setting.value("pic").toByteArray();
    bool ok = pic.loadFromData(bArr, "PNG");
    // Load the picture
    if(!ok) {
        QMessageBox::warning(0,tr("Load"),tr("Could not load Pixmap!"),QMessageBox::Ok);
        return;
    }
    updatePix();
    int h = pic.height();
    int w = pic.width();
    paint.get()->setZero(Vector2d(w/2, h/2));
    this->settings.get()->setPicWidth(w);
    this->settings.get()->setPicHeight(h);

    // Load GameMode
    gameMode = setting.value("GameMode").toInt();

    // Load the players
    curPlayer = setting.value("curPlayer").toInt();

    setting.beginGroup("Players");
    QStringList players = setting.childGroups();
    player.get()->clear();  // Delete all player before loading the new player.

    onePlanet.get()->clear();
    otherPlanet.get()->clear();
    allPlanet.get()->clear();
    for(int i=0; i<players.size(); ++i) {
        setting.beginGroup(QString("Player %1").arg(i));
        QString name = setting.value("Name").toString();
        QColor col(setting.value("Color").toString());
        double v = setting.value("Velocity").toDouble();
        double a = setting.value("Angle").toDouble();
        Player p(name, col, v, a);
        player.get()->append(p);
        // Target Planets
        setting.beginGroup("Targets");

        switch(gameMode) {
        case 1:
            onePlanet.get()->insert(i, setting.value("planet").toInt());
            break;
        case 2:
            otherPlanet.get()->insert(i, setting.value("planet").toInt());
            break;
        case 3:
            QList<int> *targets = new QList<int>();
            QStringList groups = setting.childGroups();
            for(int j=0; j<groups.size(); ++j) {
                targets->append(setting.value(QString("Target %1").arg(j)).toInt());
            }
            allPlanet.get()->insert(i, targets);
        }
        setting.endGroup(); // Targets
        setting.endGroup(); // Player i
    }
    setting.endGroup();

    //Load Planets
    setting.beginGroup("Planets");
    planetList.get()->clear();  // Delete all planets before loading the new planet.
    Planet p;
    QStringList planets = setting.childGroups();
    for(int i=0; i<planets.size(); ++i) {
        setting.beginGroup(QString("Planet %1").arg(i));
        double x = setting.value("x").toDouble();
        double y = setting.value("y").toDouble();
        double r = setting.value("radius").toDouble();
        p = Planet(x, y, r);
        planetList.get()->append(p);
        setting.endGroup();
    }
    setting.endGroup();

    goToPlayer(curPlayer);

}

void Game::loadStdPic() {
    pic.load(QDir::currentPath().append("/StdPic.png"),"PNG");
    this->updatePix();
    curPlayer -= 1;
    nextPlayer();
}

void Game::saveStdPic() {
    pic.save(QDir::currentPath().append("/StdPic.png"),"PNG");
}

void Game::renewPic() {
    // Create a new playfield

    pic.fill(Qt::black);
    paint.get()->plotPlanets();
    this->imgLab->setPixmap(pic);
    this->saveStdPic();
}

void Game::initGame() {
    /* Initializes the maps for all three game modes:
    onePlanet: All player have to reach the same planet.
    otherPlanet: Every player has to reach an own planet.
    allPlanet: Every player has to reach all planets in any order
    */

    int numOfPlanets = planetList.get()->size();
    int planet_num_all = QRandomGenerator::global()->bounded(0, numOfPlanets);
    QList<int> *temp = new QList<int>();
    for(int i=0; i<numOfPlanets; ++i) {
        temp->append(i);
    }

    int planet_num = 0;
    for(int i=0; i<player.get()->count(); ++i) {
        onePlanet.get()->insert(i, planet_num_all);
        allPlanet.get()->insert(i, new QList<int>(temp->begin(), temp->end()));
        planet_num = QRandomGenerator::global()->bounded(0, numOfPlanets);
        otherPlanet.get()->insert(i, planet_num);
        qDebug() << "Game::initGame: planet_num: " << planet_num;
    }

}

void Game::checkForWin() {
    // update QMaps and check for a winner

    int colPlNr = sim.get()->getColPlNr();
    if(colPlNr!=-1) {  // If there was a collision
        switch(gameMode) {
        case 1:
            if(onePlanet.get()->value(curPlayer)==colPlNr) {
                onePlanet.get()->insert(curPlayer, -1);
                QMessageBox::information(this, "Gewonnen", "Spieler"+
                                         QVariant(curPlayer+1).toString()+" gewinnt onePlanet.");
                gameMode = -1;
            }
            break;
        case 2:
            if(otherPlanet.get()->value(curPlayer) == colPlNr) {
                otherPlanet.get()->insert(curPlayer, -1);
                QMessageBox::information(this, "Gewonnen", "Spieler"+
                                         QVariant(curPlayer+1).toString()+" gewinnt otherPlanet.");
                gameMode = -1;
            }
            break;
        case 3:
            allPlanet.get()->value(curPlayer)->removeOne(colPlNr);
            if(allPlanet.get()->value(curPlayer)->isEmpty()) {
                allPlanet.get()->value(curPlayer)->append(-1);
                QMessageBox::information(this, "Gewonnen","Spieler"+
                                         QVariant(curPlayer+1).toString()+" gewinnt allPlanet.");
                gameMode = -1;
            }
            break;
        default:
            throw "Game Mode unknown";
        }
        if(gameMode == -1) {
            goBut->setDisabled(true);
            vLine->setDisabled(true);
            aLine->setDisabled(true);
        }
        sim.get()->resetColPlNr();
    }

}

void Game::visualize_direction(const QString &str) {
    if(str=="") return;
    QString aStr = str;
    if(str=="-") aStr = "0";
    bool worked;
    float angle = QVariant(aStr).toFloat(&worked);
    if(!worked) {
        QMessageBox::warning(0, tr("Wrong value"),"Invalid value!");
    }

    copy_pic = QPixmap(pic);
    paint.get()->plot_visual_direction(&copy_pic, angle*pi2/360,
                                       player.get()->at(curPlayer).getCol());
    this->imgLab->setPixmap(copy_pic);
}
