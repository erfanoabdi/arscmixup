#!/usr/vin/env python
# coding: utf-8
"""
压缩和解压zip文件
"""

import os
import zipfile
import sys
import getopt
import platform

def compress(zip_file, input_dir):
    print '[*] begin to zip %s' %zip_file
    f_zip = zipfile.ZipFile(zip_file, 'w')
    for root, dirs, files in os.walk(input_dir):
        for f in files:
            if zip_file.find(f) >= 0 :
                continue
            # 获取文件相对路径，在压缩包内建立相同的目录结构
            abs_path = os.path.join(os.path.join(root, f))
            rel_path = os.path.relpath(abs_path, input_dir)
            f_zip.write(abs_path, rel_path, zipfile.ZIP_DEFLATED)

def extract(zip_file, output_dir):
    f_zip = zipfile.ZipFile(zip_file, 'r')

    # 解压所有文件到指定目录
    f_zip.extractall(output_dir)

    # 逐个解压文件到指定目录
#     for f in f_zip.namelist():
#         f_zip.extract(f, os.path.join(output_dir, 'bak'))

def printdir(zip_file):
    f_zip = zipfile.ZipFile(zip_file, 'r')
    print '== printdir() ============================'
    f_zip.printdir()
    print
    print '== namelist() ============================'
    for f in f_zip.namelist():
        print f
        
def usage():
        print 'usage :'
        print '           arscmaker -t [python | bin] -p path -o [mixup | parse]'
        print '           -t: type, -p: path, -o: operate'
        print '           -t python:'
        print '           use python zip tools'
        print '           -t bin:'
        print '               use system bin zip tools'
        print '           path:'
        print '               like /tmp/test/a.apk(mixup apk) or /tmp/test/(parse resources.arsc)'
        print '           -o mixup:'
        print '               mixup apk res and create a new apk, path must contain apk, like a.apk'
        print '           -o parse:'
        print '               only parse resources.arsc,if path contains apk, it will unzip and parse res, and remove it at last'
        print '           ex:'
        print '               arscmaker -t python -p /tmp/test/a.apk -o mixup'
        sys.exit(-1)
    
def delete_file_folder(src):  
    '''delete files and folders''' 
    if os.path.isfile(src):  
        try:  
            os.remove(src)  
        except:  
            pass 
    elif os.path.isdir(src):  
        for item in os.listdir(src):  
            itemsrc=os.path.join(src,item)  
            delete_file_folder(itemsrc)  
        try:  
            os.rmdir(src)  
        except:  
            pass
        

def use_python_parse(apkpath, unzip_path, apkname):
    delete_file_folder(unzip_path)
    print '[*] begin to unzip %s to %s' %(apkname, ppath)
    extract(apkpath, unzip_path)
    print '[*] unzip %s finish' %apkname
     
     
def test_platform():
    sysstr = platform.system()
    if(sysstr =="Windows"):
        return 1
    else:
        return 2
    
         
if __name__ == '__main__':
    if(len(sys.argv) < 7) :
        usage()
        
    isTypePython = True
    isOperMixup = True
    isPathConApk = False
    apkpath = ""
    
    opts, args = getopt.getopt(sys.argv[1:], "t:p:o:")
    for op, value in opts:
        print ("[*] %s, %s" %(op, value))
        if op == "-t":
            if(value != 'python' and value != 'bin'):
                usage()
            if(value == 'bin'):
                isTypePython = False
        elif op == "-p":
            apkpath = value
        elif op == "-o":
            if(value != 'parse' and value != 'mixup'):
                usage()
            if(value == 'mixup' and apkpath.find('.apk') < 0):
                usage();
            if(value == 'parse'):
                isOperMixup = False
        
    if(apkpath.find('.apk') >= 0):
            isPathConApk = True
    
    print("[*] isTypePython = %d, isOperMixup = %d, isPathConApk = %d, apkpath = %s" %(isTypePython, isOperMixup, isPathConApk, apkpath))

    apkpath_cp = apkpath
    ppath = apkpath_cp[0:apkpath_cp.rfind('/')]
    apkpath_cp = apkpath
    apkname = apkpath_cp[(apkpath_cp.rfind('/') + 1):]
    apkpath_cp = apkpath
    apkname_noapk = apkpath_cp[(apkpath_cp.rfind('/') + 1): apkpath_cp.find('.')]
    unzip_path = ppath + '/' + apkname_noapk
    new_apkname = ppath + '/' + apkname_noapk + '.new.apk'
    print '[*] parent path = %s, apkname = %s' %(ppath, apkname)
    print '[*] new apk name = %s, unzip path = %s' %(new_apkname, unzip_path)

#     homedir = os.getcwd()
#     homedir = homedir.replace('\\','/')
#     print '[*] home dir = %s' %homedir
    
    pl = test_platform()
    bin_name = 'arscparser'
    if(pl == 1):
        bin_name += '.exe'
    
    print '[*] bin name = %s' % bin_name
    
    command = bin_name + ' -t '
     
    if(isTypePython):
        command += 'python -p ' + unzip_path +' -o '
    else:
        command += 'bin -p ' + apkpath + ' -o '
     
    if(isOperMixup):
        command += 'mixup'
    else:
        command += 'parse'
            
    print '[*] command = %s' %command
    
    if(isTypePython) :
        if(isPathConApk):
            use_python_parse(apkpath, unzip_path, apkname)
            if(os.system(command) != 0):
                print '[-] exec command %s fail' %command
                sys.exit(-1)
            
        if(isOperMixup):
            resource_path = unzip_path +'/' + 'resources.arsc'
            resource_path1 = resource_path + '.1'
            metainf_path = unzip_path +'/' + 'META-INF'
            res_path = unzip_path +'/' + 'res'
            print '[*] meta inf path = %s\n[*] resource path = %s\n[*] res path = %s' %(metainf_path, resource_path, res_path)
            print '[*] resource1 path = %s' %resource_path1
            os.remove(resource_path)
            delete_file_folder(metainf_path)
            delete_file_folder(res_path)
            os.rename(resource_path1, resource_path)
            compress(new_apkname, unzip_path)
        else:
            if(os.system(command) != 0):
                print '[-] exec command %s fail' %command
                sys.exit(-1) 
            
        #remove unzip file
        if(isPathConApk):    
            delete_file_folder(unzip_path)
        
    else:
        if(os.system(command) != 0):
            print '[-] exec command %s fail' %command
            sys.exit(-1)