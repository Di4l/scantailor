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

#include "Settings.h"
#include "PageId.h"
#include "PageSequence.h"
#include "Params.h"
#include "Margins.h"
#include "Alignment.h"
#include "RelinkablePath.h"
#include "AbstractRelinker.h"
#include "MultiIndexContainer.h"
#include <QSizeF>
#include <QMutex>
#include <QMutexLocker>
#include <algorithm>
#include <functional> // for std::greater<>
#include <vector>
#include <stddef.h>

namespace page_layout
{

class Settings::Item
{
public:
	PageId pageId;
	Margins hardMarginsMM;
	QSizeF contentSizeMM;
	Alignment alignment;
	
	Item(PageId const& page_id, Margins const& hard_margins_mm,
		QSizeF const& content_size_mm, Alignment const& alignment);
	
	double hardWidthMM() const;
	
	double hardHeightMM() const;
	
	double influenceHardWidthMM() const;
	
	double influenceHardHeightMM() const;
	
	bool alignedWithOthers() const { return !alignment.isNull(); }
};


class Settings::ModifyMargins
{
public:
	ModifyMargins(Margins const& margins_mm) : m_marginsMM(margins_mm) {}
	
	void operator()(Item& item) {
		item.hardMarginsMM = m_marginsMM;
	}
private:
	Margins m_marginsMM;
};


class Settings::ModifyAlignment
{
public:
	ModifyAlignment(Alignment const& alignment) : m_alignment(alignment) {}
	
	void operator()(Item& item) {
		item.alignment = m_alignment;
	}
private:
	Alignment m_alignment;
};


class Settings::ModifyContentSize
{
public:
	ModifyContentSize(QSizeF const& content_size_mm)
	: m_contentSizeMM(content_size_mm) {}
	
	void operator()(Item& item) {
		item.contentSizeMM = m_contentSizeMM;
	}
private:
	QSizeF m_contentSizeMM;
};


class Settings::Impl
{
public:
	Impl();
	
	~Impl();
	
	void clear();

	void performRelinking(AbstractRelinker const& relinker);
	
	void removePagesMissingFrom(PageSequence const& pages);
	
	bool checkEverythingDefined(
		PageSequence const& pages, PageId const* ignore) const;
	
	std::unique_ptr<Params> getPageParams(PageId const& page_id) const;
	
	void setPageParams(PageId const& page_id, Params const& params);
	
	Params updateContentSizeAndGetParams(
		PageId const& page_id, QSizeF const& content_size_mm,
		QSizeF* agg_hard_size_before, QSizeF* agg_hard_size_after);
	
	Margins getHardMarginsMM(PageId const& page_id) const;
	
	void setHardMarginsMM(PageId const& page_id, Margins const& margins_mm);
	
	Alignment getPageAlignment(PageId const& page_id) const;
	
	AggregateSizeChanged setPageAlignment(
		PageId const& page_id, Alignment const& alignment);
	
	AggregateSizeChanged setContentSizeMM(
		PageId const& page_id, QSizeF const& content_size_mm);
	
	void invalidateContentSize(PageId const& page_id);
	
	QSizeF getAggregateHardSizeMM() const;
	
	QSizeF getAggregateHardSizeMMLocked() const;
	
	QSizeF getAggregateHardSizeMM(
		PageId const& page_id, QSizeF const& hard_size_mm,
		Alignment const& alignment) const;
private:
	struct SequencedTag {};
	struct PageIdTag {};
	struct DescWidthTag {};
	struct DescHeightTag {};
	
	// PageId extractor
	struct PageIdExtractor
	{
		using key_type = PageId;
		PageId operator()(Item const& item) const { return item.pageId; }
	};
	
	// Composite key extractor for Width ordering (alignedWithOthers DESC, hardWidthMM DESC)
	struct WidthKeyExtractor
	{
		using key_type = std::pair<bool, double>;
		
		static key_type extract(Item const& item)
		{
			// We use std::pair<bool, double>, and with std::greater<> comparator,
			// this will order by: alignedWithOthers DESC, then hardWidthMM DESC
			return {item.alignedWithOthers(), item.hardWidthMM()};
		}
	};
	
	// Composite key extractor for Height ordering (alignedWithOthers DESC, hardHeightMM DESC)
	struct HeightKeyExtractor
	{
		using key_type = std::pair<bool, double>;
		
		static key_type extract(Item const& item)
		{
			return {item.alignedWithOthers(), item.hardHeightMM()};
		}
	};
	
	
	typedef st::multi_index::multi_index_container<
		Item,
		st::multi_index::indexed_by<
			st::multi_index::ordered_unique_index<Item, PageIdTag, PageIdExtractor>,
			st::multi_index::sequenced_index<Item, SequencedTag>,
			st::multi_index::ordered_non_unique_index<Item, DescWidthTag, WidthKeyExtractor, std::greater<std::pair<bool, double>>>,
			st::multi_index::ordered_non_unique_index<Item, DescHeightTag, HeightKeyExtractor, std::greater<std::pair<bool, double>>>
		>
	> Container;

	typedef Container::index<PageIdTag>::type PageIdOrder;
	typedef Container::index<SequencedTag>::type UnorderedItems;
	typedef Container::index<DescWidthTag>::type DescWidthOrder;
	typedef Container::index<DescHeightTag>::type DescHeightOrder;
	
	mutable QMutex m_mutex;
	Container m_items;
	UnorderedItems& m_unorderedItems;
	DescWidthOrder& m_descWidthOrder;
	DescHeightOrder& m_descHeightOrder;
	QSizeF const m_invalidSize;
	Margins const m_defaultHardMarginsMM;
	Alignment const m_defaultAlignment;

	std::vector<Item>::iterator storageIterator(PageIdOrder::iterator it);
	void replaceItem(PageIdOrder::iterator it, Item const& item);
};


/*=============================== Settings ==================================*/

Settings::Settings()
:	m_ptrImpl(new Impl())
{
}

Settings::~Settings()
{
}

void
Settings::clear()
{
	return m_ptrImpl->clear();
}

void
Settings::performRelinking(AbstractRelinker const& relinker)
{
	m_ptrImpl->performRelinking(relinker);
}

void
Settings::removePagesMissingFrom(PageSequence const& pages)
{
	m_ptrImpl->removePagesMissingFrom(pages);
}

bool
Settings::checkEverythingDefined(
	PageSequence const& pages, PageId const* ignore) const
{
	return m_ptrImpl->checkEverythingDefined(pages, ignore);
}

std::unique_ptr<Params>
Settings::getPageParams(PageId const& page_id) const
{
	return m_ptrImpl->getPageParams(page_id);
}

void
Settings::setPageParams(PageId const& page_id, Params const& params)
{
	return m_ptrImpl->setPageParams(page_id, params);
}

Params
Settings::updateContentSizeAndGetParams(
	PageId const& page_id, QSizeF const& content_size_mm,
	QSizeF* agg_hard_size_before, QSizeF* agg_hard_size_after)
{
	return m_ptrImpl->updateContentSizeAndGetParams(
		page_id, content_size_mm,
		agg_hard_size_before, agg_hard_size_after
	);
}

Margins
Settings::getHardMarginsMM(PageId const& page_id) const
{
	return m_ptrImpl->getHardMarginsMM(page_id);
}

void
Settings::setHardMarginsMM(PageId const& page_id, Margins const& margins_mm)
{
	m_ptrImpl->setHardMarginsMM(page_id, margins_mm);
}

Alignment
Settings::getPageAlignment(PageId const& page_id) const
{
	return m_ptrImpl->getPageAlignment(page_id);
}

Settings::AggregateSizeChanged
Settings::setPageAlignment(PageId const& page_id, Alignment const& alignment)
{
	return m_ptrImpl->setPageAlignment(page_id, alignment);
}

Settings::AggregateSizeChanged
Settings::setContentSizeMM(
	PageId const& page_id, QSizeF const& content_size_mm)
{
	return m_ptrImpl->setContentSizeMM(page_id, content_size_mm);
}

void
Settings::invalidateContentSize(PageId const& page_id)
{
	return m_ptrImpl->invalidateContentSize(page_id);
}

QSizeF
Settings::getAggregateHardSizeMM() const
{
	return m_ptrImpl->getAggregateHardSizeMM();
}

QSizeF
Settings::getAggregateHardSizeMM(
	PageId const& page_id, QSizeF const& hard_size_mm,
	Alignment const& alignment) const
{
	return m_ptrImpl->getAggregateHardSizeMM(page_id, hard_size_mm, alignment);
}


/*============================== Settings::Item =============================*/

Settings::Item::Item(
	PageId const& page_id, Margins const& hard_margins_mm,
	QSizeF const& content_size_mm, Alignment const& align)
:	pageId(page_id),
	hardMarginsMM(hard_margins_mm),
	contentSizeMM(content_size_mm),
	alignment(align)
{
}

double
Settings::Item::hardWidthMM() const
{
	return contentSizeMM.width() + hardMarginsMM.left() + hardMarginsMM.right();
}

double
Settings::Item::hardHeightMM() const
{
	return contentSizeMM.height() + hardMarginsMM.top() + hardMarginsMM.bottom();
}

double
Settings::Item::influenceHardWidthMM() const
{
	return alignment.isNull() ? 0.0 : hardWidthMM();
}

double
Settings::Item::influenceHardHeightMM() const
{
	return alignment.isNull() ? 0.0 : hardHeightMM();
}


/*============================= Settings::Impl ==============================*/


Settings::Impl::Impl()
:	m_items(),
	m_unorderedItems(m_items.get<SequencedTag>()),
	m_descWidthOrder(m_items.get<DescWidthTag>()),
	m_descHeightOrder(m_items.get<DescHeightTag>()),
	m_invalidSize(),
	m_defaultHardMarginsMM(page_layout::Settings::defaultHardMarginsMM()),
	m_defaultAlignment(Alignment::TOP, Alignment::HCENTER)
{
}

Settings::Impl::~Impl()
{
}

void
Settings::Impl::clear()
{
	QMutexLocker const locker(&m_mutex);
	m_items.clear();
}

void
Settings::Impl::performRelinking(AbstractRelinker const& relinker)
{
	QMutexLocker locker(&m_mutex);
	Container new_items;

	for (Item const& item : m_unorderedItems) {
		RelinkablePath const old_path(item.pageId.imageId().filePath(), RelinkablePath::File);
		Item new_item(item);
		new_item.pageId.imageId().setFilePath(relinker.substitutionPathFor(old_path));
		new_items.insert(new_item);
	}

	m_items.swap(new_items);
}

void
Settings::Impl::removePagesMissingFrom(PageSequence const& pages)
{
	QMutexLocker const locker(&m_mutex);

	std::vector<PageId> sorted_pages;
	size_t const num_pages = pages.numPages();
	sorted_pages.reserve(num_pages);
	for (size_t i = 0; i < num_pages; ++i) {
		sorted_pages.push_back(pages.pageAt(i).id());
	}
	std::sort(sorted_pages.begin(), sorted_pages.end());

	UnorderedItems::iterator it(m_unorderedItems.begin());
	UnorderedItems::iterator const end(m_unorderedItems.end());
	while (it != end) {
		if (std::binary_search(sorted_pages.begin(), sorted_pages.end(), it->pageId)) {
			++it;
		} else {
			m_unorderedItems.erase(it++);
		}
	}
}

bool
Settings::Impl::checkEverythingDefined(
	PageSequence const& pages, PageId const* ignore) const
{
	QMutexLocker const locker(&m_mutex);
	
	size_t const num_pages = pages.numPages();
	for (size_t i = 0; i < num_pages; ++i) {
		PageInfo const& page_info = pages.pageAt(i);
		if (ignore && *ignore == page_info.id()) {
			continue;
		}
		PageIdOrder const& page_index = m_items.get<PageIdTag>();
		PageIdOrder::const_iterator const it(page_index.find(page_info.id()));
		if (it == page_index.end() || !it->contentSizeMM.isValid()) {
			return false;
		}
	}
	
	return true;
}

std::unique_ptr<Params>
Settings::Impl::getPageParams(PageId const& page_id) const
{
	QMutexLocker const locker(&m_mutex);
	
	PageIdOrder const& page_index = m_items.get<PageIdTag>();
	PageIdOrder::const_iterator const it(page_index.find(page_id));
	if (it == page_index.end()) {
		return std::unique_ptr<Params>();
	}
	
	return std::unique_ptr<Params>(
		new Params(it->hardMarginsMM, it->contentSizeMM, it->alignment)
	);
}

void
Settings::Impl::setPageParams(PageId const& page_id, Params const& params)
{
	QMutexLocker const locker(&m_mutex);
	
	Item const new_item(
		page_id, params.hardMarginsMM(),
		params.contentSizeMM(), params.alignment()
	);
	
	PageIdOrder& page_index = m_items.get<PageIdTag>();
	PageIdOrder::iterator it(page_index.lower_bound(page_id));
	if (it == page_index.end() || page_id < it->pageId) {
		m_items.insert(new_item);
	} else {
		replaceItem(it, new_item);
	}
}

Params
Settings::Impl::updateContentSizeAndGetParams(
	PageId const& page_id, QSizeF const& content_size_mm,
	QSizeF* agg_hard_size_before, QSizeF* agg_hard_size_after)
{
	QMutexLocker const locker(&m_mutex);
	
	if (agg_hard_size_before) {
		*agg_hard_size_before = getAggregateHardSizeMMLocked();
	}
	
	PageIdOrder& page_index = m_items.get<PageIdTag>();
	PageIdOrder::iterator it(page_index.lower_bound(page_id));
	Item item(
		page_id, m_defaultHardMarginsMM,
		content_size_mm, m_defaultAlignment
	);
	if (it == page_index.end() || page_id < it->pageId) {
		m_items.insert(item);
	} else {
		item = *it;
		item.contentSizeMM = content_size_mm;
		replaceItem(it, item);
	}

	if (agg_hard_size_after) {
		*agg_hard_size_after = getAggregateHardSizeMMLocked();
	}

	return Params(
		item.hardMarginsMM,
		item.contentSizeMM, item.alignment
	);
}

Margins
Settings::Impl::getHardMarginsMM(PageId const& page_id) const
{
	QMutexLocker const locker(&m_mutex);
	
	PageIdOrder const& page_index = m_items.get<PageIdTag>();
	PageIdOrder::const_iterator const it(page_index.find(page_id));
	if (it == page_index.end()) {
		return m_defaultHardMarginsMM;
	} else {
		return it->hardMarginsMM;
	}
}

void
Settings::Impl::setHardMarginsMM(
	PageId const& page_id, Margins const& margins_mm)
{
	QMutexLocker const locker(&m_mutex);
	
	PageIdOrder& page_index = m_items.get<PageIdTag>();
	PageIdOrder::iterator it(page_index.lower_bound(page_id));
	if (it == page_index.end() || page_id < it->pageId) {
		Item const item(
			page_id, margins_mm, m_invalidSize, m_defaultAlignment
		);
		m_items.insert(item);
	} else {
		Item item(*it);
		item.hardMarginsMM = margins_mm;
		replaceItem(it, item);
	}
}

Alignment
Settings::Impl::getPageAlignment(PageId const& page_id) const
{
	QMutexLocker const locker(&m_mutex);
	
	PageIdOrder const& page_index = m_items.get<PageIdTag>();
	PageIdOrder::const_iterator const it(page_index.find(page_id));
	if (it == page_index.end()) {
		return m_defaultAlignment;
	} else {
		return it->alignment;
	}
}

Settings::AggregateSizeChanged
Settings::Impl::setPageAlignment(
	PageId const& page_id, Alignment const& alignment)
{
	QMutexLocker const locker(&m_mutex);
	
	QSizeF const agg_size_before(getAggregateHardSizeMMLocked());

	PageIdOrder& page_index = m_items.get<PageIdTag>();
	PageIdOrder::iterator it(page_index.lower_bound(page_id));
	if (it == page_index.end() || page_id < it->pageId) {
		Item const item(
			page_id, m_defaultHardMarginsMM, m_invalidSize, alignment
		);
		m_items.insert(item);
	} else {
		Item item(*it);
		item.alignment = alignment;
		replaceItem(it, item);
	}

	QSizeF const agg_size_after(getAggregateHardSizeMMLocked());
	if (agg_size_before == agg_size_after) {
		return AGGREGATE_SIZE_UNCHANGED;
	} else {
		return AGGREGATE_SIZE_CHANGED;
	}
}

Settings::AggregateSizeChanged
Settings::Impl::setContentSizeMM(
	PageId const& page_id, QSizeF const& content_size_mm)
{
	QMutexLocker const locker(&m_mutex);
	
	QSizeF const agg_size_before(getAggregateHardSizeMMLocked());
	
	PageIdOrder& page_index = m_items.get<PageIdTag>();
	PageIdOrder::iterator it(page_index.lower_bound(page_id));
	if (it == page_index.end() || page_id < it->pageId) {
		Item const item(
			page_id, m_defaultHardMarginsMM,
			content_size_mm, m_defaultAlignment
		);
		m_items.insert(item);
	} else {
		Item item(*it);
		item.contentSizeMM = content_size_mm;
		replaceItem(it, item);
	}
	
	QSizeF const agg_size_after(getAggregateHardSizeMMLocked());
	if (agg_size_before == agg_size_after) {
		return AGGREGATE_SIZE_UNCHANGED;
	} else {
		return AGGREGATE_SIZE_CHANGED;
	}
}

void
Settings::Impl::invalidateContentSize(PageId const& page_id)
{
	QMutexLocker const locker(&m_mutex);
	
	PageIdOrder& page_index = m_items.get<PageIdTag>();
	PageIdOrder::iterator it(page_index.find(page_id));
	if (it != page_index.end()) {
		Item item(*it);
		item.contentSizeMM = m_invalidSize;
		replaceItem(it, item);
	}
}

QSizeF
Settings::Impl::getAggregateHardSizeMM() const
{
	QMutexLocker const locker(&m_mutex);
	return getAggregateHardSizeMMLocked();
}

QSizeF
Settings::Impl::getAggregateHardSizeMMLocked() const
{
	if (m_items.empty()) {
		return QSizeF(0.0, 0.0);
	}
	
	Item const& max_width_item = *m_descWidthOrder.begin();
	Item const& max_height_item = *m_descHeightOrder.begin();
	
	double const width = max_width_item.influenceHardWidthMM();
	double const height = max_height_item.influenceHardHeightMM();
	
	return QSizeF(width, height);
}

QSizeF
Settings::Impl::getAggregateHardSizeMM(
	PageId const& page_id, QSizeF const& hard_size_mm,
	Alignment const& alignment) const
{
	if (alignment.isNull()) {
		return getAggregateHardSizeMM();
	}
	
	QMutexLocker const locker(&m_mutex);
	
	if (m_items.empty()) {
		return QSizeF(0.0, 0.0);
	}
	
	double width = 0.0;
	
	{
		DescWidthOrder::iterator it(m_descWidthOrder.begin());
		if (it->pageId != page_id) {
			width = it->influenceHardWidthMM();
		} else {
			++it;
			if (it == m_descWidthOrder.end()) {
				width = hard_size_mm.width();
			} else {
				width = std::max(
					hard_size_mm.width(), qreal(it->influenceHardWidthMM())
				);
			}
		}
	}
	
	double height = 0.0;
	
	{
		DescHeightOrder::iterator it(m_descHeightOrder.begin());
		if (it->pageId != page_id) {
			height = it->influenceHardHeightMM();
		} else {
			++it;
			if (it == m_descHeightOrder.end()) {
				height = hard_size_mm.height();
			} else {
				height = std::max(
					hard_size_mm.height(), qreal(it->influenceHardHeightMM())
				);
			}
		}
	}
	
	return QSizeF(width, height);
}

std::vector<Settings::Item>::iterator
Settings::Impl::storageIterator(PageIdOrder::iterator it)
{
	Item const* target = &(*it);
	std::vector<Item>& storage = m_items.storage();
	return std::find_if(
		storage.begin(), storage.end(),
		[target](Item const& entry) { return &entry == target; }
	);
}

void
Settings::Impl::replaceItem(PageIdOrder::iterator it, Item const& item)
{
	std::vector<Item>::iterator storage_it(storageIterator(it));
	if (storage_it != m_items.storage().end()) {
		m_items.erase(storage_it);
	}
	m_items.insert(item);
}

} // namespace page_layout
