#include "termproperties.h"

TermProperties::TermProperties(QString workingDir)
{
    this->insert(WorkingDir, workingDir);
}

TermProperties::TermProperties(QString workingDir, QString colorScheme)
{
    this->insert(WorkingDir, workingDir);
    this->insert(ColorScheme, colorScheme);
}

TermProperties::TermProperties(std::initializer_list<std::pair<TermProperty, QVariant> > list)
    : QMap<TermProperty, QVariant>(list)
{

}
