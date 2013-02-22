{-# LANGUAGE Haskell2010 #-}

-- Copyright (C) 2012-2013 Thiago Leucz Astrizi

-- This is the source code of src2html, an auxiliary program used to
-- help in the creation of html documentation of C source code. It was
-- created to help the documentation of the Weaver Framework.

-- src2html is free software: you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation, either version 3 of the License, or
-- (at your option) any later version.
--
-- src2html is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with src2html.  If not, see <http://www.gnu.org/licenses/>.


import System.IO
import Data.List
import Data.Maybe

-- This program is used to create documentation from a source code.
-- It gets as input a C source code and prints a HTML snipplet
-- thar represents the code.

src2html :: String -> String
src2html c = unlines $ map unwords' html_list
  where c_list = map words $ lines c
        html_list = transform' c_list

unwords' :: [String] -> String
unwords' a
  | isInfixOf "<span class=\"comentario\">" (unwords a) = (unwords a)++"</span>"
  | otherwise = unwords a

transform' :: [[String]] -> [[String]]
transform' a = map transform'' a

transform'' :: [String] -> [String]
transform'' a = map transform a

transform :: String -> String
transform "void" = "<span class=\"tipo\">void</span>"
transform "int" = "<span class=\"tipo\">int</span>"
transform "float" = "<span class=\"tipo\">float</span>"
transform "char" = "<span class=\"tipo\">char</span>"
transform "unsigned" = "<span class=\"tipo\">unsigned</span>"
transform "signed" = "<span class=\"tipo\">signed</span>"
transform "short" = "<span class=\"tipo\">short</span>"
transform "long" = "<span class=\"tipo\">long</span>"
transform "double" = "<span class=\"tipo\">double</span>"
transform "struct" = "<span class=\"tipo\">struct</span>"
transform "union" = "<span class=\"tipo\">union</span>"
transform "if" = "<span class=\"palavra\">if</span>"
transform "for" = "<span class=\"palavra\">for</span>"
transform "while" = "<span class=\"palavra\">while</span>"
transform "do" = "<span class=\"palavra\">do</span>"
transform "break" = "<span class=\"palavra\">break</span>"
transform "continue" = "<span class=\"palavra\">continue</span>"
transform "return" = "<span class=\"palavra\">return</span>"
transform "else" = "<span class=\"palavra\">else</span>"
transform ('#':a) = "<span class=\"precompil\">#" ++ a ++ "</span>"
transform ('<':a) = "&lt;" ++ (transform a)
transform ('&':a) = "&amp;" ++ (transform a)
transform ('/':'/':a) = "<span class=\"comentario\">" ++ ('/':'/':a)
transform a
  | a /= [] && last a == ';' = (transform (init a)) ++ ";"
  | a /= [] && last a == '{' = (transform (init a)) ++ "{"
  | a /= [] && head a == '(' && last a == ')' = "(" ++ (transform (init (tail a))) ++ ")"
  | a /= [] && last a == ')' = (transform (init a)) ++ ")"
  | a /= [] && head a == '(' = "(" ++ (transform (tail a))
  | isInfixOf "(" a = if transform inicio == inicio
                        then "<b>" ++ inicio ++ "</b>(" ++ (transform (tail fim))
                        else (transform inicio) ++ "(" ++ (transform (tail fim))
  | otherwise = a
  where (inicio, fim) = splitAt (fromJust (findIndex (== '(') a)) a

main :: IO ()
main = interact src2html