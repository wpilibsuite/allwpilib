<?php

$server_base = 'http://users.wpi.edu/~bamiller/WPIRoboticsLibrary/';
$outdir = 'WPILib/'; // Trailing slash is REQUIRED!

// Don't Touch Below Here!!

$pg_classlist = 'annotated.html';

$members = array('TypeExtractor', 'StaticFieldExtractor', 'StaticFunctionExtractor', 'FieldExtractor', 'FunctionExtractor');
$visibilities = array('public', 'protected', 'private');

// Member Extraction Functions
function SeekBlockStart($blockHeader, $classdata) {
	$blockStartRegex = '/<tr><td colspan="2"><br><h2>' . $blockHeader . '<\/h2><\/td><\/tr>/';
	$i = 0;
	
	for( ; $i < sizeof($classdata); $i++) {
		if(preg_match($blockStartRegex, $classdata[$i]) === 1) {
			$i++;
			break;
		}
	}
	
	return $i;
}

function StaticFunctionExtractor($classdata, $visibility) {
	$blockDataRegex = '/<tr><td class="memItemLeft" nowrap align="right" valign="top">static (?:<a class="el" href=".+?">)?(?<return>.*?)(?:<\/a>)?(?: \*)?&nbsp;<\/td><td class="memItemRight" valign="bottom"><a class="el" href=".*?">(?<name>.*?)<\/a> (?<params>\(.*?\))<\/td><\/tr>/';
	
	$dispVisibility = ucfirst($visibility);
	$data = array();
	
	$i = SeekBlockStart('Static ' . $dispVisibility .' Member Functions', $classdata);
	
	for( ; $i < sizeof($classdata); $i++) {
		if(preg_match($blockDataRegex, $classdata[$i], $member) === 1) {
			$member['params'] = strip_tags($member['params']);
			$data[] = "\t{$visibility} static {$member['return']} {$member['name']} {$member['params']} {\n\n\t}\n\n";
		} elseif(stripos($classdata[$i], '<td colspan="2">') !== FALSE) {
			break;
		}
	}
	
	return $data;
}

function StaticFieldExtractor($classdata, $visibility) {
	//<tr><td class="memItemLeft" nowrap align="right" valign="top">static const long&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="class_analog_module.html#93e13c1b5a181a92a660400c5c93c85c">kTimebase</a> = 40000000</td></tr>

	$blockDataRegex = '/<tr><td class="memItemLeft" nowrap align="right" valign="top">(?<modifiers>.*?)?(?:<a class="el" href=".+?">)?(?<type>.*?)(?:<\/a>)?(?: \*)?&nbsp;<\/td><td class="memItemRight" valign="bottom"><a class="el" href=".*?">(?<name>.*?)<\/a>(?<initializer>\s*=\s*.+?)?<\/td><\/tr>/';
	
	$dispVisibility = ucfirst($visibility);
	$data = array();
	
	$i = SeekBlockStart('Static ' . $dispVisibility .' Attributes', $classdata);
	
	for( ; $i < sizeof($classdata); $i++) {
		if(preg_match($blockDataRegex, $classdata[$i], $member) === 1) {
			$data[] = "\t{$visibility} {$member['modifiers']} {$member['type']} {$member['name']}{$member['initializer']};\n";
		} elseif(stripos($classdata[$i], '<td colspan="2">') !== FALSE) {
			break;
		}
	}
	
	if(sizeof($data) > 0)
		$data[] = "\n";
	
	return $data;
}

function FunctionExtractor($classdata, $visibility) {
	$blockDataRegex = '/<tr><td class="memItemLeft" nowrap align="right" valign="top">(?:<a class="el" href=".+?">)?(?<return>.*?)(?:<\/a>)?(?: \*)?&nbsp;<\/td><td class="memItemRight" valign="bottom"><a class="el" href=".*?">(?<name>.*?)<\/a> (?<params>\(.*?\))<\/td><\/tr>/';
	
	$dispVisibility = ucfirst($visibility);
	$data = array();
	
	$i = SeekBlockStart($dispVisibility .' Member Functions', $classdata);
	
	for( ; $i < sizeof($classdata); $i++) {
		if(preg_match($blockDataRegex, $classdata[$i], $member) === 1) {
			$member['params'] = strip_tags($member['params']);
			$data[] = "\t{$visibility} {$member['return']} {$member['name']} {$member['params']} {\n\n\t}\n\n";
		} elseif(stripos($classdata[$i], '<td colspan="2">') !== FALSE) {
			break;
		}
	}
	
	return $data;
}

function FieldExtractor($classdata, $visibility) {
	$blockDataRegex = '/<tr><td class="memItemLeft" nowrap align="right" valign="top">(?:<a class="el" href=".+?">)?(?<type>.*?)(?:<\/a>)?(?: \*)?&nbsp;<\/td><td class="memItemRight" valign="bottom"><a class="el" href=".*?">(?<name>.*?)<\/a><\/td><\/tr>/';
	
	$dispVisibility = ucfirst($visibility);
	$data = array();
	
	$i = SeekBlockStart($dispVisibility .' Attributes', $classdata);
	
	for( ; $i < sizeof($classdata); $i++) {
		if(preg_match($blockDataRegex, $classdata[$i], $member) === 1) {
			$data[] = "\t{$visibility} {$member['type']} {$member['name']};\n";
		} elseif(stripos($classdata[$i], '<td colspan="2">') !== FALSE) {
			break;
		}
	}
	
	if(sizeof($data) > 0)
		$data[] = "\n";
	
	return $data;
}

function TypeExtractor($classdata, $visibility) {
	$blockDataRegex = '/<tr><td class="memItemLeft" nowrap align="right" valign="top">(?:<a class="el" href=".+?">)?(?<type>.*?)(?:<\/a>)?(?: \*)?&nbsp;<\/td><td class="memItemRight" valign="bottom"><a class="el" href=".*?">(?<name>.*?)<\/a>\s\{\s*<a class="el" href=".+?">(?<members>.+?)<\/a>\s*\}\s*<\/td><\/tr>/sm';
	
	$dispVisibility = ucfirst($visibility);
	$data = array();
	
	$i = SeekBlockStart($dispVisibility .' Types', $classdata);
	
	$activeLine = '';
	
	for( ; $i < sizeof($classdata); $i++) {
		$activeLine .= $classdata[$i];
		//echo "Matching against:\n$activeLine\n";
		if(preg_match($blockDataRegex, $activeLine, $member) === 1) {
			$member['members'] = str_replace("\n", "\n\t\t", strip_tags($member['members']));
			$data[] = "\t{$visibility} {$member['type']} {$member['name']} {\n\t\t{$member['members']}\n\t}\n\n";
			$activeLine = '';
		} elseif(stripos($classdata[$i], '<td colspan="2">') !== FALSE) {
			break;
		}
	}
	
	if(sizeof($data) > 0)
		$data[] = "\n";
	
	return $data;
}

function BlockWriter($fp, $lines) {
	foreach($lines as $line)
		fwrite($fp, $line);
}

function ClassGen($className, $fileName) {
	global $server_base, $outdir, $visibilities, $members;
	
	$classData = file($server_base . 'class' . $fileName . '.html');
	$fp = fopen($outdir . $className . '.java', w);
	
	$classHeader = <<<EOH
package edu.wpi.first.wpilibj;

import java.io.*;


EOH;
	
	fwrite($fp, $classHeader);
	
	fwrite($fp, "public class {$className}{\n");
	
	foreach($members as $m)
		foreach($visibilities as $v)
			BlockWriter($fp, $m($classData, $v));
	
	fwrite($fp, "}\n");
	fclose($fp);
}

function StructGen($className, $fileName) {
	global $server_base, $outdir, $visibilities, $members;
	
	$classData = file($server_base . 'struct' . $fileName . '.html');
	$fp = fopen($outdir . $className . '.java', w);
	
	$classHeader = <<<EOH
package edu.wpi.first.wpilibj;

import java.io.*;


EOH;
	
	fwrite($fp, $classHeader);
	
	fwrite($fp, "public struct {$className}{\n");
	
	foreach($members as $m)
		foreach($visibilities as $v)
			BlockWriter($fp, $m($classData, $v));
	
	fwrite($fp, "}\n");
	fclose($fp);
}

$classList = file_get_contents($server_base . $pg_classlist);

$classRegex = '/<tr><td class="indexkey"><a class="el" href="class(?<docname>.*?)\.html">(?<name>.*?)<\/a><\/td><td class="indexvalue">.*?<\/td><\/tr>/';
preg_match_all($classRegex, $classList, $classes, PREG_SET_ORDER);

foreach($classes as $class) {
	//echo "Found class {$class['classname']} in file class{$class['docname']}.html\n";
	echo "Generating class {$class['name']}\n";
	ClassGen($class['name'], $class['docname']);
}

$structRegex = '/<tr><td class="indexkey"><a class="el" href="struct(?<docname>.*?)\.html">(?<name>.*?)<\/a><\/td><td class="indexvalue">.*?<\/td><\/tr>/';
preg_match_all($structRegex, $classList, $structs, PREG_SET_ORDER);

foreach($structs as $struct) {
	//echo "Found class {$class['classname']} in file class{$class['docname']}.html\n";
	echo "Generating struct {$struct['name']}\n";
	StructGen($struct['name'], $struct['docname']);
}

//ClassGen('Accelerometer', '_accelerometer');
//ClassGen('AnalogModule', '_analog_module');
//ClassGen('Relay', '_relay');

?>