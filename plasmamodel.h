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
    void setElectronConcDistribution(QVector<double>);
    void setIonConcDistribution(QVector<double>);
    void setElectronsNumber(QVector<double>);
    void setIonsNumber(QVector<double>);

    QVector<double> getR(); //returns r vector
    QVector<double> getPhiDistribution(); //returns phi vector
    QVector<double> getElectronConcDistribution();//returns ne
    QVector<double> getIonConcDistribution();//returns ni
    QVector<double> getElectronsNumber(); //returns np_e
    QVector<double> getIonsNumber();//returns np_i

private:
    QVector<double> r;   //radial coordinate
    QVector<double> phi; //potential
    QVector<double> ne;// radial distribution of electron concentration
    QVector<double> ni;// radial distribution of ion concentration
    QVector<double> np_e;// number of electrons
    QVector<double> np_i;// number of ions

signals:

public slots:
};

#endif // PLASMAMODEL_H
