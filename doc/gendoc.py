import os
import string

def getExt(fname):
    n = fname.rfind('.')
    return fname[n:]

def add_home_link(s):
    home_link = "#### [Home](README.md)\n"
    if s.find(home_link) == -1:
        s = home_link + s
    return s


def gen_doc(root_dir):
    print(root_dir)
    for dir_name, _, file_list in os.walk(root_dir):
        for fname in file_list:
            if fname == "README.md" or getExt(fname) != ".md":
                continue

            print(fname)
            try:
                s = open(dir_name + '/' + fname, 'r').read()
            except UnicodeDecodeError:
                failFiles.append(dir_name + '/' + fname)
                err = True

            s = add_home_link(s)

            f = open(dir_name + '/' + fname, 'w')
            f.write(s)
            f.close()

rdir = input("Doc folder: ")
if rdir == ".":
    rdir = os.getcwd()
if os.path.isdir(rdir):
    gen_doc(rdir)