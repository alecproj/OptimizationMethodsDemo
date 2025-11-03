//
// Created on 3 Nov, 2025
//  by alecproj
//

#ifndef SOURCES_APPENUMS_HPP_
#define SOURCES_APPENUMS_HPP_

#include <QObject>
#include <cassert>

class ExtremumType : public QObject {
    Q_OBJECT
public:
    enum Type {
        MINIMUM  = 0,
        MAXIMUM  = 1
    };
    Q_ENUM(Type)

    explicit ExtremumType(QObject *parent = nullptr) : QObject(parent) {}
};

class AlgoType : public QObject {
    Q_OBJECT
public:
    enum Type {
        ALL = 0,
        CD  = 1, // Coordinate Descent
        GD  = 2  // Gradient Descent
    };
    Q_ENUM(Type)

    explicit AlgoType(QObject *parent = nullptr) : QObject(parent) {}
};

class ExtensionType : public QObject {
    Q_OBJECT
public:
    enum Type {
        B   = 0, // Basic
        S  = 1,  // Steepest descent method
        R  = 2   // Ravine method
    };
    Q_ENUM(Type)

    explicit ExtensionType(QObject *parent = nullptr) : QObject(parent) {}
};

class FullAlgoType : public QObject {
    Q_OBJECT
public:
    enum Type {
        INVALID = 0,
        CDB     = 1, // Coordinate Descent Basic
        CDS     = 2, // Coordinate Descent Steepest
        GDB     = 3, // Gradient Descent Basic
        GDS     = 4, // Gradient Descent Steepest
        GDR     = 5  // Gradient Descent Ravine
    };
    Q_ENUM(Type)

    explicit FullAlgoType(QObject *parent = nullptr) : QObject(parent) {}
};

class CheckList : public QObject {
    Q_OBJECT
public:
    enum Check { 
        Error               = -1,
        CalcAccuracy        = (1 << 0),  // CDB,CDS,GDB,GDS,GDR
        ResultAccuracy      = (1 << 1),  // CDB,CDS,GDB,GDS,GDR
        StartX1             = (1 << 2),  // CDB,CDS,GDB,GDS,GDR 
        StartY1             = (1 << 3),  // CDB,CDS,GDB,GDS,GDR 
        StartX2             = (1 << 4),  //    ,   ,   ,   ,GDR
        StartY2             = (1 << 5),  //                ,GDR
        StepX               = (1 << 6),  // CDB,   ,   ,   ,
        StepY               = (1 << 7),  // CDB,   ,   ,   ,
        CoefficientStep     = (1 << 8),  //    ,   ,GDB,   ,GDR
        MinX                = (1 << 9),  // CDB,CDS,GDB,GDS,GDR
        MaxX                = (1 << 10), // CDB,CDS,GDB,GDS,GDR
        MinY                = (1 << 11), // CDB,CDS,GDB,GDS,GDR
        MaxY                = (1 << 12), // CDB,CDS,GDB,GDS,GDR
        Iterations          = (1 << 13), // CDB,CDS,GDB,GDS,GDR 

        CDBCheck            = 16079,
        CDSCheck            = 15887,
        GDBCheck            = 16143,
        GDSCheck            = 15887,
        GDRCheck            = 16191
    };
    Q_ENUM(Check)

    explicit CheckList(QObject *parent = nullptr) : QObject(parent) {
        static_assert(CDBCheck == (
            CalcAccuracy + ResultAccuracy + StartX1 + StartY1 +
            StepX        + StepY          + MinX    + MaxX    + 
            MinY         + MaxY           + Iterations
        ), "Invalid CDBCheck");
        static_assert(CDSCheck == (
            CalcAccuracy + ResultAccuracy + StartX1 + StartY1 +
            MinX         + MaxX           + MinY    + MaxY    +
            Iterations
        ), "Invalid CDSCheck");
        static_assert(GDBCheck == (
            CalcAccuracy    + ResultAccuracy + StartX1 + StartY1 +
            CoefficientStep + MinX           + MaxX    + MinY    +
            MaxY            + Iterations
        ), "Invalid GDBCheck");
        static_assert(GDSCheck == (
            CalcAccuracy + ResultAccuracy + StartX1 + StartY1 +
            MinX         + MaxX           + MinY    + MaxY    +
            Iterations
        ), "Invalid GDSCheck");
        static_assert(GDRCheck == (
            CalcAccuracy + ResultAccuracy + StartX1 + StartY1 +
            StartX2      + StartY2        + CoefficientStep   +
            MinX         + MaxX           + MinY    + MaxY    +
            Iterations
        ), "Invalid GDRCheck");
    }

};

class EnumHelper : public QObject {
    Q_OBJECT
public:

    explicit EnumHelper(QObject *parent = nullptr) : QObject(parent) {}

    Q_INVOKABLE FullAlgoType::Type calculateFullType(AlgoType::Type algo, ExtensionType::Type extension)
    {
        if (algo == AlgoType::CD) {
            if (extension == ExtensionType::B) {
                return FullAlgoType::CDB;
            } else if (extension == ExtensionType::S) {
                return FullAlgoType::CDS;
            }
        } else if (algo == AlgoType::GD) {
            if (extension == ExtensionType::B) {
                return FullAlgoType::GDB;
            } else if (extension == ExtensionType::S) {
                return FullAlgoType::GDS;
            } else if (extension == ExtensionType::R) {
                return FullAlgoType::GDR;
            }
        }
        return FullAlgoType::INVALID;
    }

    Q_INVOKABLE CheckList::Check getCheckByFullType(FullAlgoType::Type type) 
    {
        if (type == FullAlgoType::CDB) {
            return CheckList::CDBCheck;
        } else if (type == FullAlgoType::CDS) {
            return CheckList::CDSCheck;
        } else if (type == FullAlgoType::GDB) {
            return CheckList::GDBCheck;
        } else if (type == FullAlgoType::GDS) {
            return CheckList::GDSCheck;
        } else if (type == FullAlgoType::GDR) {
            return CheckList::GDRCheck;
        }
        return CheckList::Error;
    }
};


#endif // SOURCES_APPENUMS_HPP_
