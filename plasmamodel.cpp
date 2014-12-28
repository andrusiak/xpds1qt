#include "plasmamodel.h"

PlasmaModel::PlasmaModel(QObject *parent) : QObject(parent)
{

}

PlasmaModel::~PlasmaModel()
{

}

void PlasmaModel::setR(QVector<double> r)
{
    this->r = r;
}

void PlasmaModel::setPhiDistribution(QVector<double> phi_r)
{
    this->phi = phi_r;
}

void PlasmaModel::setElectronConcDistribution(QVector<double> ne_r)
{
    this->ne = ne_r;
}

void PlasmaModel::setIonConcDistribution(QVector<double> ni_r)
{
    this->ni = ni_r;
}

QVector<double> PlasmaModel::getR()
{
    return r;
}

QVector<double> PlasmaModel::getPhiDistribution()
{
    return phi;
}

QVector<double> PlasmaModel::getElectronConcDistribution()
{
    return ne;
}

QVector<double> PlasmaModel::getIonConcDistribution()
{
    return ni;
}


