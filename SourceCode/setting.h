#ifndef SETTING_H
#define SETTING_H

#include<QDialog>
#include<QTableWidget>
#include<QDialogButtonBox>
#include<QGridLayout>
#include<QPushButton>
#include<QSpinBox>
#include<QLabel>
#include "player.h"
#include "planet.h"
#include<QStringList>
#include<QRadioButton>
#include<QGroupBox>

class Setting : public QDialog
{
    Q_OBJECT
private:
    QTableWidget *table;
    QPushButton *ok,*close, *std, *load, *save;
    QDialogButtonBox *dialButBox;
    QGridLayout *lay;
    QSpinBox *spin;
    QLabel *anzLab;
    std::shared_ptr<QVector<Player>> playerList;
    QVector<Player> loadedPlayers;
    std::shared_ptr<QStringList> names, color;
    std::shared_ptr<QVector<Planet>> planetList;
    QVector<QColor> primaryColors;
    QRadioButton *onePlanet, *otherPlanet, *allPlanet;

    //More options
    int minPlSize, maxPlSize, picWidth, picHeight, numOfPlanets;
    QGroupBox *moreOpts;
    QSpinBox *anzPlanets, *pixWidth, *pixHeight;
    QSpinBox *plSizeMin, *plSizeMax;
    QLabel *anzPl, *hwSettings, *x, *hwUnits, *sizePl, *to;
    QGridLayout *optLay;

public:
    Setting(std::shared_ptr<QVector<Player>> l, std::shared_ptr<QStringList> nam, std::shared_ptr<QStringList> col,
            std::shared_ptr<QVector<Planet>> planets, int minPlanetSize, int maxPlanetSize,
            int pixWidth, int pixHeight, int nPlanets, QWidget *parent=0);
    QTableWidget* getTable() const { return table; }
    void applySetting();
    int getGameMode();
    void fillPlanetList(int n);
    int getPicHeight() { return picHeight; }
    int getPicWidth() { return picWidth; }
    void setPicHeight(unsigned int h) { picHeight = h; }
    void setPicWidth(unsigned int w) { picWidth = w; }
    double px2m(double p) { return p*200000; }
    double m2px(double m) { return m/200000; }

public slots:
    void colDial(int i,int j);
    void editIt(int i, int j);
    void updateTable(int num);
    void writeStd();
    void load_settings();
    void save_settings();
    void load_std();

};

#endif // SETTING_H
