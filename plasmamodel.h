#ifndef PLASMAMODEL_H
#define PLASMAMODEL_H

#include <QObject>
#include <QVector>

class PlasmaModel : public QObject
{
    Q_OBJECT
public:
    explicit PlasmaModel(QObject *parent = 0);
    ~PlasmaModel();

    void setR(QVector<double>);
    void setPhiDistribution(QVector<double>);
    void setIonConcDistribution(QVector<double>);
    void setElectronConcDistribution(QVector<double>);

    QVector<double> getR(); //returns r vector
    QVector<double> getPhiDistribution(); //returns phi vector
    QVector<double> getIonConcDistribution();//returns ne
    QVector<double> getElectronConcDistribution();//returns ni

private:
    QVector<double> r;   //radial coordinate
    QVector<double> phi; //Potential
    QVector<double> ne;// radial distribution of electron concentration
    QVector<double> ni;// radial distribution of ion concentration


signals:

public slots:
};

#endif // PLASMAMODEL_H
