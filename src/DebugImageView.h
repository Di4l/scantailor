/*
	Scan Tailor - Interactive post-processing tool for scanned pages.
	Copyright (C)  Joseph Artsimovich <joseph.artsimovich@gmail.com>

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DEBUG_IMAGE_VIEW_H_
#define DEBUG_IMAGE_VIEW_H_

#include "qt/AutoRemovingFile.h"
#include <QStackedWidget>
#include <list>

class QImage;

// NOTE: DebugImageView was previously part of boost::intrusive::list.
// Now managed separately. Can be re-implemented as intrusive_list in future.
class DebugImageView :
	public QStackedWidget
{
public:
	DebugImageView(AutoRemovingFile file, QWidget* parent = 0);

	/**
	 * Tells this widget to either display the actual image or just
	 * a placeholder.
	 */
	void setLive(bool live);
private:
	class ImageLoadResult;
	class ImageLoader;

	void imageLoaded(QImage const& image);

	AutoRemovingFile m_file;
	QWidget* m_pPlaceholderWidget;
	bool m_isLive;
};

#endif
