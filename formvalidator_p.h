#ifndef INPUTVIEWVALIDATORPRIVATE_H
#define INPUTVIEWVALIDATORPRIVATE_H

#include <QObject>
#include <QVector>
#include <QPalette>
#include <QMap>

class ValidityRule;
class FormValidator;

typedef QVector<ValidityRule*> ValidityRules;

class FormValidatorPrivate : public QObject {
    Q_OBJECT
public:
    FormValidatorPrivate(FormValidator* qq);
    ~FormValidatorPrivate();
    FormValidator* q_ptr;

public Q_SLOTS:
    void lineEditTextChanged(const QString&);
    void spinBoxValueChanged(int);
    void comboboxIndexChanged(int);

private:
    bool isValidationEnabled;
    bool isAutoFocusEnabled;

    QMap<QWidget*, ValidityRules> widgetRules;
    Q_DECLARE_PUBLIC(FormValidator)
};

#endif // INPUTVIEWVALIDATORPRIVATE_H
