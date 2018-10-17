#pragma once
#include <QMainWindow>

namespace ASAP::Worklist::GUI
{
	class CompositeChild : public QMainWindow
	{
		Q_OBJECT
		public:
			CompositeChild(QWidget *parent);

			virtual QMenuBar* GetMenuElement(void) = 0;

	/*	public slots:
			virtual void IsCompositeChild(void) = 0;

		signals:
			void RequiresContextSwitch(const std::string context_id);*/
	};
}