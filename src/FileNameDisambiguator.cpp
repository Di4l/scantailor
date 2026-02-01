#include <functional>
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

#include "FileNameDisambiguator.h"
#include "RelinkablePath.h"
#include "AbstractRelinker.h"
#include "MultiIndexContainer.h"
#include <QString>
#include <QFileInfo>
#include <QDomDocument>
#include <QDomElement>
#include <QMutex>
#include <functional>
#include <tuple>
#include <limits>

class FileNameDisambiguator::Impl
{
public:
	Impl();

	Impl(QDomElement const& disambiguator_el,
		std::function<QString(QString const&)> const& file_path_unpacker);

	QDomElement toXml(QDomDocument& doc, QString const& name,
		std::function<QString(QString const&)> const& file_path_packer) const;

	int getLabel(QString const& file_path) const;

	int registerFile(QString const& file_path);

	void performRelinking(AbstractRelinker const& relinker);
private:
	struct ItemsByFilePathTag {};
	struct ItemsByFileNameLabelTag {};
	struct UnorderedItemsTag {};

	struct Item
	{
		QString filePath;
		QString fileName;
		int label;

		Item(QString const& file_path, int lbl);

		Item(QString const& file_path, QString const& file_name, int lbl);
	};

	using FilePathExtractor = st::multi_index::member<Item, QString, &Item::filePath>;
	using FileNameExtractor = st::multi_index::member<Item, QString, &Item::fileName>;
	using LabelExtractor = st::multi_index::member<Item, int, &Item::label>;

	using Container = st::multi_index::multi_index_container<
		Item,
		st::multi_index::indexed_by<
			st::multi_index::ordered_unique_index<
				Item,
				ItemsByFilePathTag,
				FilePathExtractor
			>,
			st::multi_index::ordered_unique_index<
				Item,
				ItemsByFileNameLabelTag,
				st::multi_index::composite_key<FileNameExtractor, LabelExtractor>
			>,
			st::multi_index::sequenced_index<Item, UnorderedItemsTag>
		>
	>;

	using ItemsByFilePath = st::multi_index::ordered_unique_index<
		Item,
		ItemsByFilePathTag,
		FilePathExtractor
	>;
	using ItemsByFileNameLabel = st::multi_index::ordered_unique_index<
		Item,
		ItemsByFileNameLabelTag,
		st::multi_index::composite_key<FileNameExtractor, LabelExtractor>
	>;
	using UnorderedItems = st::multi_index::sequenced_index<Item, UnorderedItemsTag>;

	mutable QMutex m_mutex;
	Container m_items;
};


/*====================== FileNameDisambiguator =========================*/

FileNameDisambiguator::FileNameDisambiguator()
:	m_ptrImpl(new Impl)
{
}

FileNameDisambiguator::FileNameDisambiguator(
	QDomElement const& disambiguator_el)
:	m_ptrImpl(new Impl(disambiguator_el, [](const QString& s) { return s; }))
{
}

FileNameDisambiguator::FileNameDisambiguator(
	QDomElement const& disambiguator_el,
	std::function<QString(QString const&)> const& file_path_unpacker)
:	m_ptrImpl(new Impl(disambiguator_el, file_path_unpacker))
{
}

QDomElement
FileNameDisambiguator::toXml(QDomDocument& doc, QString const& name) const
{
	return m_ptrImpl->toXml(doc, name, [](const QString& s) { return s; });
}

QDomElement
FileNameDisambiguator::toXml(
	QDomDocument& doc, QString const& name,
	std::function<QString(QString const&)> const& file_path_packer) const
{
	return m_ptrImpl->toXml(doc, name, file_path_packer);
}

int
FileNameDisambiguator::getLabel(QString const& file_path) const
{
	return m_ptrImpl->getLabel(file_path);
}

int
FileNameDisambiguator::registerFile(QString const& file_path)
{
	return m_ptrImpl->registerFile(file_path);
}

void
FileNameDisambiguator::performRelinking(AbstractRelinker const& relinker)
{
	m_ptrImpl->performRelinking(relinker);
}


/*==================== FileNameDisambiguator::Impl ====================*/

FileNameDisambiguator::Impl::Impl()
:	m_items()
{
}

FileNameDisambiguator::Impl::Impl(
	QDomElement const& disambiguator_el,
	std::function<QString(QString const&)> const& file_path_unpacker)
:	m_items()
{
	QDomNode node(disambiguator_el.firstChild());
	for (; !node.isNull(); node = node.nextSibling())
	{
		if (!node.isElement())
		{
			continue;
		}
		if (node.nodeName() != "mapping")
		{
			continue;
		}
		QDomElement const file_el(node.toElement());

		QString const file_path_shorthand(file_el.attribute("file"));
		QString const file_path = file_path_unpacker(file_path_shorthand);
		if (file_path.isEmpty()) {
			// Unresolved shorthand - skipping this record.
			continue;
		}

		int const label = file_el.attribute("label").toInt();
		m_items.insert(Item(file_path, label));
	}
}

QDomElement
FileNameDisambiguator::Impl::toXml(
	QDomDocument& doc, QString const& name,
	std::function<QString(QString const&)> const& file_path_packer) const
{
	QMutexLocker const locker(&m_mutex);

	QDomElement el(doc.createElement(name));

	auto& unorderedItems = m_items.get<UnorderedItemsTag>();
	for (Item const& item : unorderedItems)
	{
		QString const file_path_shorthand = file_path_packer(item.filePath);
		if (file_path_shorthand.isEmpty())
		{
			// Unrepresentable file path - skipping this record.
			continue;
		}
		
		QDomElement file_el(doc.createElement("mapping"));
		file_el.setAttribute("file", file_path_shorthand);
		file_el.setAttribute("label", item.label);
		el.appendChild(file_el);
	}

	return el;
}

int
FileNameDisambiguator::Impl::getLabel(QString const& file_path) const
{
	QMutexLocker const locker(&m_mutex);

	auto& itemsByFilePath = m_items.get<ItemsByFilePathTag>();
	auto fp_it = itemsByFilePath.find(file_path);
	if (fp_it != itemsByFilePath.end())
	{
		return fp_it->label;
	}

	return 0;
}

int
FileNameDisambiguator::Impl::registerFile(QString const& file_path)
{
	QMutexLocker const locker(&m_mutex);

	auto& itemsByFilePath = m_items.get<ItemsByFilePathTag>();
	auto fp_it = itemsByFilePath.find(file_path);
	if (fp_it != itemsByFilePath.end())
	{
		return fp_it->label;
	}

	int label = 0;

	QString const file_name(QFileInfo(file_path).fileName());
	auto& itemsByFileNameLabel = m_items.get<ItemsByFileNameLabelTag>();
	auto fn_it = itemsByFileNameLabel.upper_bound(std::make_tuple(file_name, std::numeric_limits<int>::min()));
	
	// If the item preceeding fn_it has the same file name,
	// the new file belongs to the same disambiguation group.
	if (fn_it != itemsByFileNameLabel.begin())
	{
		auto prev = fn_it;
		--prev;
		if (prev->fileName == file_name)
		{
			label = prev->label + 1;
		}
	} // Otherwise, label remains 0.
	
	Item const new_item(file_path, file_name, label);
	m_items.insert(new_item);

	return label;
}

void
FileNameDisambiguator::Impl::performRelinking(AbstractRelinker const& relinker)
{
	QMutexLocker const locker(&m_mutex);
	Container new_items;

	auto& unorderedItems = m_items.get<UnorderedItemsTag>();
	for (Item const& item : unorderedItems)
	{
		RelinkablePath const old_path(item.filePath, RelinkablePath::File);
		Item new_item(relinker.substitutionPathFor(old_path), item.label);
		new_items.insert(new_item);
	}

	m_items.swap(new_items);
}


/*============================ Impl::Item =============================*/

FileNameDisambiguator::Impl::Item::Item(QString const& file_path, int lbl)
:	filePath(file_path),
	fileName(QFileInfo(file_path).fileName()),
	label(lbl)
{
}

FileNameDisambiguator::Impl::Item::Item(
	QString const& file_path, QString const& file_name, int lbl)
:	filePath(file_path),
	fileName(file_name),
	label(lbl)
{
}
