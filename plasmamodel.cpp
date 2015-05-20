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
void PlasmaModel::setTime(QVector<double> t)
{
    this->t = t;
}

void PlasmaModel::setPhiDistribution(QVector<double> phi_r)
{
    this->phi = phi_r;
}

void PlasmaModel::setFieldDistribution(QVector<double> e_r)
{
    this->e = e_r;
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

void PlasmaModel::setDustCharge(QVector<double> Q_d)
{
    this->Q_d = Q_d;
}

void PlasmaModel::setEnergyFluxes(QVector<double> dE_d)
{
    this->dE_d = dE_d;
}

QVector<double> PlasmaModel::getR()
{
    return r;
}

QVector<double> PlasmaModel::getTime()
{
    return t;
}

QVector<double> PlasmaModel::getPhiDistribution()
{
    return phi;
}

QVector<double> PlasmaModel::getFieldDistribution()
{
    return this->e;
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

QVector<double> PlasmaModel::getDustCharge()
{
    return Q_d;
}

QVector<double> PlasmaModel::getEnergyFluxes()
{
    return dE_d;
}
