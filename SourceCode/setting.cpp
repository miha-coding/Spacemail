#include "setting.h"
#include "player.h"
#include<QColorDialog>
#include<QTableWidgetItem>
#include<QDebug>
#include<QFile>
#include<QTextStream>
#include<QDir>
#include<QStringList>
#include<QGroupBox>
#include<QRadioButton>
#include<QHBoxLayout>
#include<QSettings>
#include<QFileDialog>
#include<QRandomGenerator>

Setting::Setting(std::shared_ptr<QVector<Player>> l, std::shared_ptr<QStringList> nam,
                 std::shared_ptr<QStringList> col, std::shared_ptr<QVector<Planet>> planets,
                 int minPlanetSize, int maxPlanetSize, int picWidth, int picHeight,
                 int nPlanets, QWidget *parent) :
    QDialog(parent), playerList(l), names(nam), color(col), planetList(planets),
    minPlSize(minPlanetSize), maxPlSize(maxPlanetSize), picWidth(picWidth),
    picHeight(picHeight), numOfPlanets(nPlanets)
{
    lay = new QGridLayout(this);
    anzLab = new QLabel(tr("Number of Player"),this);
    spin = new QSpinBox(this);
    ok = new QPushButton(tr("Start"),this);
    close = new QPushButton(tr("Close"),this);
    dialButBox = new QDialogButtonBox(this);

    std = new QPushButton(tr("Save as Standard"));
    load = new QPushButton(tr("Load"));
    save = new QPushButton(tr("Save"));

    spin->setRange(1,10);
    dialButBox->addButton(ok,QDialogButtonBox::AcceptRole);
    dialButBox->addButton(close,QDialogButtonBox::RejectRole);

    // Game Mode
    onePlanet = new QRadioButton(tr("One Planet"), this);
    otherPlanet = new QRadioButton(tr("Other Planet"), this);
    allPlanet = new QRadioButton(tr("All Planets"), this);
    QHBoxLayout *modeButLay = new QHBoxLayout(this);
    modeButLay->addWidget(onePlanet, 1);
    modeButLay->addWidget(otherPlanet, 2);
    modeButLay->addWidget(allPlanet, 3);
    QGroupBox *modeGroup = new QGroupBox(tr("Game Mode"), this);
    modeGroup->setLayout(modeButLay);
    onePlanet->setChecked(true);

    connect(ok,SIGNAL(clicked(bool)),this,SLOT(accept()));
    connect(close,SIGNAL(clicked(bool)),this,SLOT(reject()));
    connect(std,SIGNAL(clicked(bool)),this,SLOT(writeStd()));
    connect(load,SIGNAL(clicked(bool)), this, SLOT(load_settings()));
    connect(save, SIGNAL(clicked(bool)), this, SLOT(save_settings()));
    connect(spin,SIGNAL(valueChanged(int)),this,SLOT(updateTable(int)));

    table = new QTableWidget(l->size(),2,this);
    QStringList headers;
    headers << tr("Name") << tr("Color");
    table->setHorizontalHeaderLabels(headers);
    lay->addWidget(anzLab,0,1,1,2);
    lay->addWidget(spin,0,3,1,1);
    lay->addWidget(table,1,1,5,3);
    lay->addWidget(modeGroup, 6,1,1,3);
    // Row 7 to 11 below (moreOpts)
    lay->addWidget(std,12,1,1,1);
    lay->addWidget(load,12,2,1,1);
    lay->addWidget(save,12,3,1,1);
    lay->addWidget(dialButBox,13,2,1,2);


    primaryColors = QVector<QColor>();
    QColor tmpCol;
    for(int i=0;i<l->size();i++) {
        QTableWidgetItem *it = new QTableWidgetItem(l->at(i).getName());
        table->setItem(i,0,it);
        tmpCol = l->at(i).getCol();
        primaryColors.append(tmpCol);
        it =  new QTableWidgetItem(tmpCol.name());
        it->setBackground(tmpCol);
        table->setItem(i,1,it);

    }
    connect(table,SIGNAL(cellClicked(int,int)),this,SLOT(editIt(int,int)));


    // More Options

    anzPlanets = new QSpinBox(this);
    pixWidth = new QSpinBox(this);
    pixHeight = new QSpinBox(this);
    plSizeMin = new QSpinBox(this);
    plSizeMax = new QSpinBox(this);
    optLay = new QGridLayout(this);
    anzPl = new QLabel(tr("Number of Planets:"), this);
    hwSettings = new QLabel(tr("Width x Height:"), this);
    x = new QLabel("x", this);
    hwUnits = new QLabel(tr("Px"), this);
    sizePl = new QLabel(tr("Size of Planets:"), this);
    to = new QLabel(tr("to"), this);


    anzPlanets->setRange(1,25);
    anzPlanets->setValue(numOfPlanets);
    pixWidth->setRange(600,1600);
    pixWidth->setValue(picWidth);
    pixHeight->setRange(600,900);
    pixHeight->setValue(picHeight);
    plSizeMin->setRange(5,50);
    plSizeMin->setValue(minPlSize);
    plSizeMax->setRange(20,50);
    plSizeMax->setValue(maxPlSize);


    optLay->addWidget(anzPl,1,1,1,1);
    optLay->addWidget(anzPlanets,1,2,1,1);
    optLay->addWidget(sizePl,2,1,1,1);
    optLay->addWidget(plSizeMin,2,2,1,1);
    optLay->addWidget(to,2,3,1,1);
    optLay->addWidget(plSizeMax,2,4,1,1);
    optLay->addWidget(hwSettings,3,1,1,1);
    optLay->addWidget(pixWidth,3,2,1,1);
    optLay->addWidget(x,3,3,1,1);
    optLay->addWidget(pixHeight,3,4,1,1);
    optLay->addWidget(hwUnits,3,5,1,1);

    moreOpts = new QGroupBox(tr("More Options"), this);
    moreOpts->setLayout(optLay);

    moreOpts->setCheckable(true);
    moreOpts->setChecked(false);

    lay->addWidget(moreOpts, 7,1,4,3);
    loadedPlayers = QVector<Player>();
}

void Setting::updateTable(int num) {
    while(table->rowCount()>num) {
        table->removeRow(table->rowCount()-1);
    }
    while(table->rowCount()<num) {
        table->insertRow(table->rowCount());
        QTableWidgetItem *it = new QTableWidgetItem(tr("Player %1").arg(table->rowCount()));
        if (table->rowCount()<names.get()->count()) {
            it->setText(names.get()->at(table->rowCount()-1));
        }
        table->setItem(table->rowCount()-1,0,it);

        //Color
        QColor col;
        if(table->rowCount()<=primaryColors.size()) {
            col = primaryColors.at(table->rowCount()-1);
        }
        else {
            int r = rand() % 255;
            int g = rand() % 255;
            int b = rand() % 255;
            col = QColor(r,g,b);
        }
        it = new QTableWidgetItem(col.name());
        it->setBackground(col);
        if(table->rowCount()<color.get()->count() && color.get()->at(table->rowCount()-1) != "") {
            it->setText(color.get()->at(table->rowCount()-1));
            it->setBackground(QColor(color.get()->at(table->rowCount()-1)));
        }
        table->setItem(table->rowCount()-1,1,it);

    }

}

void Setting::writeStd() {
    qDebug() << "Write Std";
    QFile f(QDir::currentPath().append("/std.txt"));
    f.open(QFile::WriteOnly);
    QTextStream io(&f);
    for(int i=0;i<table->rowCount();i++) {
        io << table->item(i,0)->text() << ";" << table->item(i,1)->text() << "\n";
    }

    f.close();
}

void Setting::colDial(int i, int j) {
    if(j!=1) return;

    auto dial = std::make_unique<QColorDialog>();
    dial.get()->setCurrentColor(table->item(i,j)->background().color());
    dial.get()->exec();

    table->item(i,j)->setSelected(false);
}

void Setting::editIt(int i, int j) {
    if(j==0) {
        table->editItem(table->item(i,j));
        return;
    }
    if(j==1) {
        auto dial = std::make_unique<QColorDialog>();
        dial.get()->setCurrentColor(table->item(i,j)->background().color());
        int ret = dial.get()->exec();
        if(ret==QColorDialog::Accepted) {
            table->item(i,1)->setBackground(dial.get()->currentColor());
            table->item(i,1)->setText(dial.get()->currentColor().name());
        }

        table->item(i,j)->setSelected(false);
    }
}

void Setting::applySetting() {
    playerList.get()->clear();
    names.get()->clear();
    color.get()->clear();
    for(int i=0;i<table->rowCount();i++) {
        playerList.get()->append(Player(table->item(i,0)->text(),
            table->item(i,1)->background().color().name()));
        names.get()->append(table->item(i,0)->text());
        color.get()->append(table->item(i,1)->background().color().name());
    }
    picHeight = pixHeight->value();
    picWidth = pixWidth->value();

    minPlSize = plSizeMin->value();
    maxPlSize = plSizeMax->value();

    fillPlanetList(anzPlanets->value());
}

int Setting::getGameMode() {
    if(onePlanet->isChecked()) {
        return 1;
    }
    else if(otherPlanet->isChecked()) {
        return 2;
    }
    return 3;
}

void Setting::load_settings() {
    // Initialize
    QString loadFileName = QFileDialog::getOpenFileName(nullptr, tr("Load Settings"), QDir::currentPath(), "*.ini");
    QSettings sets(loadFileName, QSettings::IniFormat);

    //Player
    int numPlayer = sets.value("numPlayer").toInt();
    primaryColors.clear();
    spin->setValue(numPlayer);
    table->setRowCount(0);  // empty the table
    QString name, col;
    for(int i=0;i<numPlayer; ++i) {
        table->insertRow(i);
        name = (sets.value(tr("Player%1.name").arg(i)).toString());
        col = (sets.value(tr("Player%1.col").arg(i)).toString());
        table->setItem(i, 0, new QTableWidgetItem(name));
        table->setItem(i, 1, new QTableWidgetItem(col));
        table->item(i, 1)->setBackground(QColor(col));
        primaryColors.append(col);
    }

    // Game mode
    name = sets.value("Mode").toString();
    onePlanet->setChecked(false);
    otherPlanet->setChecked(false);
    allPlanet->setChecked(false);
    if(name=="onePlanet") onePlanet->setChecked(true);
    else if(name=="otherPlanet") otherPlanet->setChecked(true);
    else allPlanet->setChecked(true);

    // More Options
    anzPlanets->setValue(sets.value("NumPlanets").toInt());
    plSizeMin->setValue(sets.value("SizePlMin").toInt());
    plSizeMax->setValue(sets.value("SizePlMax").toInt());
    pixWidth->setValue(sets.value("PixWidth").toInt());
    pixHeight->setValue(sets.value("PixHeight").toInt());

}

void Setting::save_settings() {
    // Initialize
    QString saveFileName = QFileDialog::getSaveFileName(nullptr, tr("Save Settings"),QDir::currentPath(),"*.ini");
    QSettings sets(saveFileName, QSettings::IniFormat);
    qDebug() << "Setting:saveFileName path: " << saveFileName;

    // Player
    int numPlayers = spin->value();
    sets.setValue("numPlayer", numPlayers);
    QString name, col;
    for(int i=0; i<numPlayers; ++i) {
        name = table->item(i, 0)->text();
        col = table->item(i, 1)->text();
        sets.setValue(tr("Player%1.name").arg(i), name);
        sets.setValue(tr("Player%1.col").arg(i), col);
    }

    //game Mode
    if(onePlanet->isChecked()) name = "onePlanet";
    else if(otherPlanet->isChecked()) name = "otherPlanet";
    else name = "allPlanets";
    sets.setValue("Mode", name);

    //more Options
    sets.setValue("NumPlanets", anzPlanets->value());
    sets.setValue("SizePlMin", plSizeMin->value());
    sets.setValue("SizePlMax", plSizeMax->value());
    sets.setValue("PixWidth", pixWidth->value());
    sets.setValue("PixHeight", pixHeight->value());

}

void Setting::load_std() {

}

void Setting::fillPlanetList(int n) {
    /* Creates n planets at random positions and with random radius.
     *
     * Input: n (int): number of planets.
     */

    // Remove old planets and free the memory.
    planetList.get()->clear();
    //Radius: (e.g. Min: 5Px, Max: 50Px)
    double x,y,r, max_r;
    Planet pl;
    bool overlapping = false;
    int i=0;
    qDebug() << "fillPlanetList: PicSize widht= " << picWidth << " height= " << picHeight;
    while(i<n) {
        // Random location
        x = QRandomGenerator::global()->bounded(- picWidth + picWidth/2, picWidth - picWidth/2);
        y = QRandomGenerator::global()->bounded(- picHeight + picHeight/2, picHeight - picHeight/2);

        // Random radius
        if(maxPlSize>minPlSize) {
            r = QRandomGenerator::global()->bounded(minPlSize, maxPlSize);
        }
        else {
            r = maxPlSize;
        }

        // Check for distance to origin
        Vector2d zero(picWidth/2, picHeight/2);
        Vector2d drawPosition(px2m(x),px2m(y));
        if((drawPosition-zero).norm()<1.5*px2m(r)) {
            continue;
        }
        // Check for overlapping planets ('half' overlapping is ok)
        for(int j=0; j<planetList.get()->size(); ++j) {
            pl = planetList.get()->operator[](j);
            max_r = px2m(r)<pl.getRadius() ? pl.getRadius() : px2m(r);
            if((drawPosition-pl.getPosition()).norm()<max_r) {
                overlapping=true;
                break;
            }
        }
        if(overlapping) {
            overlapping=false;
            continue;
        }
        qDebug() << "Game::FillPlanetList: " << x << ", " << y << ", " << r;

        pl = Planet(px2m(x),px2m(y),px2m(r));
        planetList.get()->append(pl);
        ++i;
    }
}
