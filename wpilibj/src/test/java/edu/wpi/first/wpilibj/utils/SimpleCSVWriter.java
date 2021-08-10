package edu.wpi.first.wpilibj.utils;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.List;

public class SimpleCSVWriter {

    File m_csvOutputFile;
    String m_fname;

    List<List<String>> m_dataLines = new ArrayList<>();

    public SimpleCSVWriter(String fname, List<String> names, List<String>units){

        m_fname = fname;

        names.add(0,"TIME");
        units.add(0,"sec");

        m_dataLines.add(names);
        m_dataLines.add(units);
    }

    public void writeData(double time, List<Double> vals){
        ArrayList<String> data = new ArrayList<String>(vals.size()+1);
        data.add(Double.toString(time));
        for(Double val : vals){
            data.add(val.toString());
        }
        m_dataLines.add(data);
    }

    public void close(){
        String fpath = "./" + m_fname + ".csv";
        System.out.println("Logging run data to " + fpath);
        m_csvOutputFile = new File(fpath);
        PrintWriter pw;
        try {
            pw = new PrintWriter(m_csvOutputFile);
        } catch (FileNotFoundException e) {
            System.out.println("Could not open CSV log file");
            e.printStackTrace();
            return;
        }

        for(List<String> line :m_dataLines){
            for(String item : line){
                pw.print(item);
                pw.print(",");
            }
            pw.print("\n");
        }

        pw.close();
        
    }
    
}
