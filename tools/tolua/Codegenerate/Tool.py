
import fnmatch
import os
import sys
from pprint import pprint
from clang import cindex 
from clang.cindex import Config
from optparse import OptionParser, OptionGroup

Config.set_library_path("C:/Python27/libs/libclang/")

headerPath = "E:/WorkSpace/DDT/Engine/BaseGame/MENet/"

def iter_find_files(path, fnexp):
	for root, dirs, files, in os.walk(path):
		for filename in fnmatch.filter(files, fnexp):
			yield os.path.join(root, filename)

def showToken(node):
    ts=node.get_tokens()
    for t in ts:
        print "\t" + t.spelling

def get_diag_info(diag):
    return { 'severity' : diag.severity,
             'location' : diag.location,
             'spelling' : diag.spelling,
             'ranges' : diag.ranges,
             'fixits' : diag.fixits }

def get_cursor_id(cursor, cursor_list = []):
    if not opts.showIDs:
        return None

    if cursor is None:
        return None

    # FIXME: This is really slow. It would be nice if the index API exposed
    # something that let us hash cursors.
    for i,c in enumerate(cursor_list):
        if cursor == c:
            return i
    cursor_list.append(cursor)
    return len(cursor_list) - 1

def get_info(node, depth=0):
    if opts.maxDepth is not None and depth >= opts.maxDepth:
        children = None
    else:
        children = [get_info(c, depth+1)
                    for c in node.get_children()]
    return { 'id' : get_cursor_id(node),
             'kind' : node.kind,
             'usr' : node.get_usr(),
             'spelling' : node.spelling,
             'location' : node.location,
             'extent.start' : node.extent.start,
             'extent.end' : node.extent.end,
             'is_definition' : node.is_definition(),
             'definition id' : get_cursor_id(node.get_definition()),
             'children' : children }


def find_typerefs(node):
	"""
	Find all reference to the type named "typename"
	"""
	if node.kind.is_reference():
		ref_node = cindex.Cursor_ref(node)
		print "Type:", ref_node.spelling
		showToken(node)
	for child in node.get_children():
		find_typerefs(child)

def parse_file(filename):
	index = cindex.Index.create()

	arr = []
	arr.append(filename)

	tu = index.parse(None, arr)
	print "Translate unit:", tu.spelling
	pprint(('diags', map(get_diag_info, tu.diagnostics)))
	pprint(('nodes', get_info(tu.cursor)))

	find_typerefs(tu.cursor)

global opts
parser = OptionParser("usage: %prog [options] {filename} [clang-args*]")
parser.add_option("", "--show-ids", dest="showIDs",
                  help="Don't compute cursor IDs (very slow)",
                  default=False)
parser.add_option("", "--max-depth", dest="maxDepth",
                  help="Limit cursor expansion to depth N",
                  metavar="N", type=int, default=None)
parser.disable_interspersed_args()
(opts, args) = parser.parse_args()


for filename in iter_find_files(headerPath, "*.h"):
	print filename
	parse_file(filename)
