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

void PlasmaModel::setElectronsNumber(QVector<double> npe_r)
{
    this->np_e = npe_r;
}

void PlasmaModel::setIonsNumber(QVector<double> npi_r)
{
    this->np_i = npi_r;
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

QVector<double> PlasmaModel::getElectronsNumber()
{
    return np_e;
}

QVector<double> PlasmaModel::getIonsNumber()
{
    return np_i;
}

