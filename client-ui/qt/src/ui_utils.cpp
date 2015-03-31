#include <QApplication>
#include <QDesktopWidget>

#include "ui_utils.h"


void UIUtils :: CentreWidget (QWidget *parent_p, QWidget *child_p)
{
	const QRect parent_frame = parent_p -> frameGeometry ();
	const QRect child_frame = child_p -> frameGeometry ();

	/*
	 * Start be getting the iniital top left corner of our child widget
	 * centred over the parent widget.
	 */
	int x = parent_p -> x () + ((parent_frame.width () - child_frame.width ()) >> 1);
	int y = parent_p -> y () + ((parent_frame.height () - child_frame.height ()) >> 1);
	int i;

	/* Get our current screen dimensions */
	QDesktopWidget *desktop_p  = QApplication :: desktop ();
	QRect screen_frame = desktop_p -> screenGeometry (desktop_p -> screenNumber (parent_p));

	/*
	 * If the child widget would partially lie outside of the screen, adjust x and y until it isn't
	 */
	i = screen_frame.left ();
	if (x < i)
		{
			x = i;
		}
	else
		{
			i = screen_frame.right () - child_frame.width ();

			if (x > i)
				{
					x = i;
				}
		}

	i = screen_frame.top ();
	if (y < i)
		{
			y = i;
		}
	else
		{
			i = screen_frame.bottom () - child_frame.height ();

			if (y > i)
				{
					y = i;
				}
		}

	child_p -> move (x, y);
}

