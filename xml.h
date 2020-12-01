/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <optional>
#include <string_view>
#include <system_error>
#include <vector>
#include <Windows.h>
#include <MsXml6.h>
#include "com.h"
#include "insert_order_map.h"
#pragma comment(lib, "msxml6.lib")

namespace wl {

/// Handles XML documents using MSXML 6.0 library.
///
/// #include <xml.h>
class xml final {
public:
	/// A single XML node.
	///
	/// #include <xml.h>
	class node final {
	public:
		/// The name of this XML element.
		std::wstring name;
		/// The textual content of the element.
		std::wstring text;
		/// The attributes of this element.
		insert_order_map<std::wstring, std::wstring> attrs;
		/// The child nodes of this element.
		std::vector<node> children;

		/// Returns a vector of references to the nodes with the given name, case insensitive.
		std::vector<std::reference_wrapper<const node>>
			children_by_name(std::wstring_view elemName) const
		{
			return _children_by_name<const node>(this, elemName);
		}
		/// Returns a vector of references to the nodes with the given name, case insensitive.
		std::vector<std::reference_wrapper<node>>
			children_by_name(std::wstring_view elemName)
		{
			return _children_by_name<node>(this, elemName);
		}

		/// Returns the first child with the given name, case insensitive.
		std::optional<std::reference_wrapper<const node>>
			first_child_by_name(std::wstring_view elemName) const
		{
			return _first_child_by_name<const node>(this, elemName);
		}
		/// Returns the first child with the given name, case insensitive.
		std::optional<std::reference_wrapper<node>>
			first_child_by_name(std::wstring_view elemName)
		{
			return _first_child_by_name<node>(this, elemName);
		}

	private:
		template<typename thisT>
		[[nodiscard]] static std::vector<std::reference_wrapper<thisT>>
			_children_by_name(thisT* thiss, std::wstring_view elemName)
		{
			// Template because of const/non-const requirement.
			// https://stackoverflow.com/a/11655924/6923555

			std::vector<std::reference_wrapper<thisT>> nodes;
			for (thisT& n : thiss->children) {
				if (lstrcmpiW(n.name.c_str(), elemName.data()) == 0) {
					nodes.emplace_back(n);
				}
			}
			return nodes;
		}

		template<typename thisT>
		[[nodiscard]] static std::optional<std::reference_wrapper<thisT>>
			_first_child_by_name(thisT* thiss, std::wstring_view elemName)
		{
			// Template because of const/non-const requirement.

			for (thisT& n : thiss->children) {
				if (lstrcmpiW(n.name.c_str(), elemName.data()) == 0) {
					return {n};
				}
			}
			return std::nullopt;
		}
	};

private:
	com::lib _comLib{com::lib::init::LATER};

public:
	/// Root node of this XML document.
	node root;

	/// Default constructor.
	xml() = default;

	/// Constructor. Calls parse().
	xml(std::wstring_view xmlString) { this->parse(xmlString); }

	/// Move constructor.
	xml(xml&& other) noexcept { operator=(std::move(other)); }

	/// Move assignment operator.
	xml& operator=(xml&& other) = default;

	/// Parses an XML string and loads it in memory.
	xml& parse(std::wstring_view xmlString)
	{
		this->_comLib.initialize();
		this->root = {};

		// Create COM object for XML document.
		auto doc = com::co_create_instance<IXMLDOMDocument3>(
			CLSID_DOMDocument60, CLSCTX_INPROC_SERVER);
		doc->put_async(false);

		// Parse the XML string.
		VARIANT_BOOL vb = FALSE;

		if (HRESULT hr = doc->loadXML(
				static_cast<BSTR>(const_cast<wchar_t*>(xmlString.data())), &vb);
			FAILED(hr))
		{
			throw std::system_error(hr, std::system_category(),
				"IXMLDOMDocument3::loadXML() failed in " __FUNCTION__ "().");
		}

		// Get document element and root node.
		com::ptr<IXMLDOMElement> docElem;
		if (HRESULT hr = doc->get_documentElement(docElem.raw_pptr());
			FAILED(hr))
		{
			throw std::system_error(hr, std::system_category(),
				"IXMLDOMDocument3::get_documentElement() failed in " __FUNCTION__ "().");
		}

		auto rootNode = docElem.query_interface<IXMLDOMNode>();
		this->root = _build_node(rootNode);
		return *this;
	}

private:
	static node _build_node(com::ptr<IXMLDOMNode>& xmlDomNode)
	{
		node myNode;

		// Get node name.
		com::bstr bstrName;

		if (HRESULT hr = xmlDomNode->get_nodeName(&bstrName); FAILED(hr)) {
			throw std::system_error(hr, std::system_category(),
				"IXMLDOMNode::get_nodeName() failed in " __FUNCTION__ "().");
		}
		myNode.name = bstrName.c_str();

		// Parse attributes, if any.
		myNode.attrs = _parse_attributes(xmlDomNode);

		// Process children, if any.
		_parse_children(xmlDomNode, myNode);

		return myNode;
	}

	[[nodiscard]] static insert_order_map<std::wstring, std::wstring>
		_parse_attributes(com::ptr<IXMLDOMNode>& xmlDomNode)
	{
		com::ptr<IXMLDOMNamedNodeMap> attrs;
		xmlDomNode->get_attributes(attrs.raw_pptr());

		long attrCount = 0;
		attrs->get_length(&attrCount);

		insert_order_map<std::wstring, std::wstring> myAttrs;
		myAttrs.reserve(attrCount);

		for (long i = 0; i < attrCount; ++i) {
			com::ptr<IXMLDOMNode> attr;
			attrs->get_item(i, attr.raw_pptr());

			DOMNodeType type = NODE_INVALID;
			attr->get_nodeType(&type);
			if (type == NODE_ATTRIBUTE) {
				com::bstr bstrName;
				attr->get_nodeName(&bstrName); // get attribute name
				com::variant variNodeVal;
				attr->get_nodeValue(&variNodeVal); // get attribute value
				myAttrs[bstrName.c_str()] = variNodeVal.str(); // add hash entry
			}
		}
		return myAttrs;
	}

	static void _parse_children(com::ptr<IXMLDOMNode>& xmlDomNode, node& myNode)
	{
		VARIANT_BOOL vb = FALSE;
		xmlDomNode->hasChildNodes(&vb);

		if (vb != FALSE) {
			com::ptr<IXMLDOMNodeList> nodeList;

			if (HRESULT hr = xmlDomNode->get_childNodes(nodeList.raw_pptr());
				FAILED(hr))
			{
				throw std::system_error(hr, std::system_category(),
					"IXMLDOMNode::get_childNodes() failed in " __FUNCTION__ "().");
			}

			int childCount = 0;
			long totalCount = 0;
			nodeList->get_length(&totalCount);

			for (long i = 0; i < totalCount; ++i) {
				com::ptr<IXMLDOMNode> child;
				if (HRESULT hr = nodeList->get_item(i, child.raw_pptr()); FAILED(hr)) {
					throw std::system_error(hr, std::system_category(),
						"IXMLDOMNodeList::get_item() failed in " __FUNCTION__ "().");
				}

				// Node can be text or an actual child node.
				DOMNodeType type = NODE_INVALID;
				if (HRESULT hr = child->get_nodeType(&type); FAILED(hr)) {
					throw std::system_error(hr, std::system_category(),
						"IXMLDOMNode::get_nodeType() failed in " __FUNCTION__ "().");
				}

				if (type == NODE_TEXT) {
					com::bstr bstrText;
					xmlDomNode->get_text(&bstrText);
					myNode.text.append(bstrText.c_str()); // if text, append to current node text
				} else if (type == NODE_ELEMENT) {
					myNode.children.emplace_back(_build_node(child)); // recursively
				} else {
					// (L"Unhandled node type: %d.\n", type);
				}
			}
		} else {
			// Assumes that only a leaf node can have text.
			com::bstr bstrText;
			xmlDomNode->get_text(&bstrText); // if text, append to current node text
			myNode.text = bstrText.c_str();
		}
	}
};

}//namespace wl