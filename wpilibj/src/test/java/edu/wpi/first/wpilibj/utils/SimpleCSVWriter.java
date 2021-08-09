package edu.wpi.first.wpilibj.utils;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.List;

public class SimpleCSVWriter {

    File csvOutputFile;

    List<List<String>> dataLines = new ArrayList<>();

    public SimpleCSVWriter(List<String> names, List<String>units){
        names.add(0,"TIME");
        units.add(0,"sec");

        dataLines.add(names);
        dataLines.add(units);
    }

    public void writeData(double time, List<Double> vals){
        ArrayList<String> data = new ArrayList<String>(vals.size()+1);
        data.add(Double.toString(time));
        for(Double val : vals){
            data.add(val.toString());
        }
        dataLines.add(data);
    }

    public void close(){
        csvOutputFile = new File("./log.csv");
        PrintWriter pw;
        try {
            pw = new PrintWriter(csvOutputFile);
        } catch (FileNotFoundException e) {
            System.out.println("Could not open CSV log file");
            e.printStackTrace();
            return;
        }

        for(List<String> line :dataLines){
            for(String item : line){
                pw.print(item);
                pw.print(",");
            }
            pw.print("\n");
        }

        pw.close();
        
    }
    
}
