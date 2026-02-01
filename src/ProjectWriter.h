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

#ifndef PROJECTWRITER_H_
#define PROJECTWRITER_H_

#include "IntrusivePtr.h"
#include "PageSequence.h"
#include "OutputFileNameGenerator.h"
#include "ImageId.h"
#include "PageId.h"
#include "SelectedPage.h"
#include "VirtualFunction.h"
#include "MultiIndexContainer.h"
#include <QString>
#include <Qt>
#include <vector>
#include <map>

class AbstractFilter;
class ProjectPages;
class PageInfo;
class QDomDocument;
class QDomElement;

class ProjectWriter
{
	DECLARE_NON_COPYABLE(ProjectWriter)
public:
	typedef IntrusivePtr<AbstractFilter> FilterPtr;
	
	ProjectWriter(
		IntrusivePtr<ProjectPages> const& page_sequence,
		SelectedPage const& selected_page,
		OutputFileNameGenerator const& out_file_name_gen);
	
	~ProjectWriter();
	
	bool write(QString const& file_path, std::vector<FilterPtr> const& filters) const;
	
	/**
	 * \p out will be called like this: out(ImageId, numeric_image_id)
	 */
	template<typename OutFunc>
	void enumImages(OutFunc out) const;
	
	/**
	 * \p out will be called like this: out(LogicalPageId, numeric_page_id)
	 */
	template<typename OutFunc>
	void enumPages(OutFunc out) const;
private:
	struct Directory
	{
		QString path;
		int numericId;
		
		Directory(QString const& path, int numeric_id)
		: path(path), numericId(numeric_id) {}
	};
	
	struct File
	{
		QString path;
		int numericId;
		
		File(QString const& path, int numeric_id)
		: path(path), numericId(numeric_id) {}
	};
	
	struct Image
	{
		ImageId id;
		int numericId;
		int numSubPages;
		bool leftHalfRemoved;
		bool rightHalfRemoved;
		
		Image(PageInfo const& page_info, int numeric_id);
	};
	
	struct Page
	{
		PageId id;
		int numericId;
		
		Page(PageId const& id, int numeric_id)
		: id(id), numericId(numeric_id) {}
	};
	
	struct DirectoryByPathTag {};
	struct FileByPathTag {};
	struct ImageByIdTag {};
	struct PageByIdTag {};
	
	struct SequencedTag {};
	
	typedef std::map<ImageId, ImageMetadata> MetadataByImage;
	
	// DirectoryByPath extractor
	struct DirectoryPathExtractor
	{
		QString operator()(Directory const& d) const { return d.path; }
	};
	
	// FileByPath extractor
	struct FilePathExtractor
	{
		QString operator()(File const& f) const { return f.path; }
	};
	
	// ImageById extractor
	struct ImageIdExtractor
	{
		ImageId operator()(Image const& i) const { return i.id; }
	};
	
	// PageById extractor
	struct PageIdExtractor
	{
		PageId operator()(Page const& p) const { return p.id; }
	};
	
	using namespace st::multi_index;
	
	typedef multi_index_container<
		Directory,
		indexed_by<
			ordered_unique_index<Directory, DirectoryByPathTag, DirectoryPathExtractor>,
			sequenced_index<Directory, SequencedTag>
		>
	> Directories;
	
	typedef multi_index_container<
		File,
		indexed_by<
			ordered_unique_index<File, FileByPathTag, FilePathExtractor>,
			sequenced_index<File, SequencedTag>
		>
	> Files;
	
	typedef multi_index_container<
		Image,
		indexed_by<
			ordered_unique_index<Image, ImageByIdTag, ImageIdExtractor>,
			sequenced_index<Image, SequencedTag>
		>
	> Images;
	
	typedef multi_index_container<
		Page,
		indexed_by<
			ordered_unique_index<Page, PageByIdTag, PageIdExtractor>,
			sequenced_index<Page, SequencedTag>
		>
	> Pages;
	
	QDomElement processDirectories(QDomDocument& doc) const;
	
	QDomElement processFiles(QDomDocument& doc) const;
	
	QDomElement processImages(QDomDocument& doc) const;
	
	QDomElement processPages(QDomDocument& doc) const;
	
	void writeImageMetadata(
		QDomDocument& doc, QDomElement& image_el,
		ImageId const& image_id) const;
	
	int dirId(QString const& dir_path) const;
	
	int fileId(QString const& file_path) const;

	QString packFilePath(QString const& file_path) const;
	
	int imageId(ImageId const& image_id) const;
	
	int pageId(PageId const& page_id) const;
	
	void enumImagesImpl(VirtualFunction2<void, ImageId const&, int>& out) const;
	
	void enumPagesImpl(VirtualFunction2<void, PageId const&, int>& out) const;
	
	PageSequence m_pageSequence;
	OutputFileNameGenerator m_outFileNameGen;
	SelectedPage m_selectedPage;
	Directories m_dirs;
	Files m_files;
	Images m_images;
	Pages m_pages;
	MetadataByImage m_metadataByImage;
	Qt::LayoutDirection m_layoutDirection;
};

template<typename OutFunc>
void
ProjectWriter::enumImages(OutFunc out) const
{
	ProxyFunction2<OutFunc, void, ImageId const&, int> proxy(out);
	enumImagesImpl(proxy);
}

template<typename OutFunc>
void
ProjectWriter::enumPages(OutFunc out) const
{
	ProxyFunction2<OutFunc, void, PageId const&, int> proxy(out);
	enumPagesImpl(proxy);
}

#endif
