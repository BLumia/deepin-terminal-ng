#ifndef TERMPROPERTIES_H
#define TERMPROPERTIES_H

#include <QVariant>

enum TermProperty {
    WorkingDir,
    ColorScheme,
    ShellProgram
};

class TermProperties : public QMap<TermProperty, QVariant>
{
    Q_GADGET
public:
    TermProperties(QString workingDir);
    TermProperties(QString workingDir, QString colorScheme);
    TermProperties(std::initializer_list<std::pair<TermProperty,QVariant> > list);
    TermProperties(const TermProperties &) = default;
};

#endif // TERMPROPERTIES_H
