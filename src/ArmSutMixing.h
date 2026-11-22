#pragma once

struct SpreadsheetTable;

struct CourseTransportMix {
    double viscosity = 0.0;     // Pa s
    double conductivity = 0.0;  // W/(m K)
    bool usesArmalySutton = false;
};

CourseTransportMix computeCourseTransportMix(const SpreadsheetTable& species);
