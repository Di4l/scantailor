/*
	Scan Tailor - Interactive post-processing tool for scanned pages.
	Copyright (C) 2007-2009  Joseph Artsimovich <joseph_a@mail.ru>

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

#ifndef INTERACTION_STATE_H_
#define INTERACTION_STATE_H_

#include "NonCopyable.h"
#include "qt/Proximity.h"
#include <list>
#include <algorithm>
#include <QCursor>
#include <QString>

class Proximity;

class InteractionState
{
	DECLARE_NON_COPYABLE(InteractionState)
public:
	class Captor
	{
		// NOTE: Previously inherited from boost::intrusive::list_base_hook with auto_unlink.
		// Now managed by std::list. Captor holds reference to InteractionState for unlink support.
		friend class InteractionState;
	private:
		struct CopyHelper
		{
			Captor* captor;

			CopyHelper(Captor* cap) : captor(cap) {}
		};

		InteractionState* m_owner;

		void swap_nodes(Captor& other) {
			std::swap(m_proximityCursor, other.m_proximityCursor);
			std::swap(m_interactionCursor, other.m_interactionCursor);
			std::swap(m_proximityStatusTip, other.m_proximityStatusTip);
			std::swap(m_interactionStatusTip, other.m_interactionStatusTip);
		}

		void unlink();

		bool is_linked() const { return m_owner != nullptr; }

	public:
		Captor() : m_owner(nullptr) {}

		Captor(Captor& other) : m_owner(nullptr) { swap_nodes(other); }

		Captor(CopyHelper other) : m_owner(nullptr) { swap_nodes(*other.captor); }

		Captor& operator=(Captor& other);

		Captor& operator=(CopyHelper other);

		operator CopyHelper() { return CopyHelper(this); }

		void release() { unlink(); }

		QCursor const& proximityCursor() const { return m_proximityCursor; }

		void setProximityCursor(QCursor const& cursor) { m_proximityCursor = cursor; }

		QCursor const& interactionCursor() const { return m_interactionCursor; }

		void setInteractionCursor(QCursor const& cursor) { m_interactionCursor = cursor; }

		QString const& proximityStatusTip() const { return m_proximityStatusTip; }

		void setProximityStatusTip(QString const& tip) { m_proximityStatusTip = tip; }

		QString const& interactionStatusTip() const { return m_interactionStatusTip; }

		void setInteractionStatusTip(QString const& tip) { m_interactionStatusTip = tip; }

		QString const& interactionOrProximityStatusTip() const {
			return m_interactionStatusTip.isNull() ? m_proximityStatusTip : m_interactionStatusTip;
		}
	private:
		QCursor m_proximityCursor;
		QCursor m_interactionCursor;
		QString m_proximityStatusTip;
		QString m_interactionStatusTip;
	};

	InteractionState();

	void capture(Captor& captor);

	bool captured() const { return !m_captorList.empty(); }

	bool capturedBy(Captor const& captor) const;

	void resetProximity();

	void updateProximity(
		Captor& captor, Proximity const& proximity,
		int priority = 0, Proximity proximity_threshold = Proximity());

	bool proximityLeader(Captor const& captor) const;

	Proximity const& proximityThreshold() const { return m_proximityThreshold; }

	QCursor cursor() const;

	QString statusTip() const;

	QString const& defaultStatusTip() const { return m_defaultStatusTip; }

	void setDefaultStatusTip(QString const& status_tip) { m_defaultStatusTip = status_tip; }

	bool redrawRequested() const { return m_redrawRequested; }

	void setRedrawRequested(bool requested) { m_redrawRequested = requested; }
private:
	// NOTE: Was boost::intrusive::list. Now using std::list.
	// If performance becomes critical, consider implementing as intrusive_list again.
	typedef std::list<Captor*> CaptorList;

	/**
	 * Returns true if the provided proximity is better than the stored one.
	 */
	bool betterProximity(Proximity const& proximity, int priority) const;

	QString m_defaultStatusTip;
	CaptorList m_captorList;
	CaptorList m_proximityLeader;
	Proximity m_bestProximity;
	Proximity m_proximityThreshold;
	int m_bestProximityPriority;
	bool m_redrawRequested;
};

#endif
