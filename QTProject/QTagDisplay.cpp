#include "QTagDisplay.h"


QTagDisplay::QTagDisplay(QWidget *parent) :
QWidget(parent)
{
	tagLayout = new QHBoxLayout;
	tagLabel = new QLabel(tr("Tag of Object:"));
	tagEdit = new QLineEdit;
	tagEdit->setMaxLength(10);
	weightSpinBox = new QSpinBox;
	weightSpinBox->setMaximum(10);
	weightSpinBox->setMinimum(1);
	weightLabel = new QLabel(tr("Object weight:"));
	saveButton = new QPushButton(tr("Save"));
	tagLayout->addWidget(tagLabel);
	tagLayout->addWidget(tagEdit);
	tagLayout->addWidget(weightLabel);
	tagLayout->addWidget(weightSpinBox);
	tagLayout->addWidget(saveButton);
	//tagWidget = new QWidget;
	this->setLayout(tagLayout);

	connect(saveButton, SIGNAL(clicked()), this, SLOT(ClickedSaveButton())); 
}

QTagDisplay::~QTagDisplay()
{

}

void QTagDisplay::ClickedSaveButton()
{
	//emit 
}