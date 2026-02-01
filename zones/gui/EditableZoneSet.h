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

#ifndef EDITABLE_ZONE_SET_H_
#define EDITABLE_ZONE_SET_H_

#include "EditableSpline.h"
#include "PropertySet.h"
#include "IntrusivePtr.h"
#include <QObject>
#include <type_traits>
#include <map>
// NOTE: Was using boost::iterator_facade.
// Now using simple iterator wrapper compatible with C++23 range-based for loops.

class EditableZoneSet : public QObject
{
	Q_OBJECT
private:
	typedef std::map<EditableSpline::Ptr, IntrusivePtr<PropertySet> > Map;
public:
	class const_iterator;

	class Zone
	{
		friend class EditableZoneSet::const_iterator;
	public:
		Zone() {}

		EditableSpline::Ptr const& spline() const { return m_iter->first; }

		IntrusivePtr<PropertySet> const& properties() const { return m_iter->second; }
	private:
		explicit Zone(Map::const_iterator it) : m_iter(it) {}

		Map::const_iterator m_iter;
	};

	class const_iterator
	{
		friend class EditableZoneSet;
	public:
		using difference_type = std::ptrdiff_t;
		using value_type = Zone;
		using pointer = const Zone*;
		using reference = const Zone&;
		using iterator_category = std::forward_iterator_tag;

		const_iterator() : m_zone() {}

		const_iterator& operator++() { ++m_zone.m_iter; return *this; }
		const_iterator operator++(int) { const_iterator tmp(*this); ++m_zone.m_iter; return tmp; }

		bool operator==(const_iterator const& other) const {
			return m_zone.m_iter == other.m_zone.m_iter;
		}
		bool operator!=(const_iterator const& other) const {
			return m_zone.m_iter != other.m_zone.m_iter;
		}

		const Zone& operator*() const { return m_zone; }
		const Zone* operator->() const { return &m_zone; }

	private:
		explicit const_iterator(Map::const_iterator it) : m_zone(it) {}

		Zone m_zone;
	};

	typedef const_iterator iterator;

	EditableZoneSet();

	const_iterator begin() const { return iterator(m_splineMap.begin()); }

	const_iterator end() const { return iterator(m_splineMap.end()); }

	PropertySet const& defaultProperties() const { return m_defaultProps; }

	void setDefaultProperties(PropertySet const& props);

	void addZone(EditableSpline::Ptr const& spline);

	void addZone(EditableSpline::Ptr const& spline, PropertySet const& props);

	void removeZone(EditableSpline::Ptr const& spline);

	void commit();

	IntrusivePtr<PropertySet> propertiesFor(EditableSpline::Ptr const& spline);

	IntrusivePtr<PropertySet const> propertiesFor(EditableSpline::Ptr const& spline) const;
signals:
	void committed();
private:
	Map m_splineMap;
	PropertySet m_defaultProps;
};

#endif
