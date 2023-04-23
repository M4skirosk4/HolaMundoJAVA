package Pom2;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

public class PomodoroTimer extends JFrame {
    private Timer timer;
    private JLabel timeLabel;
    private JButton startButton;
    private JButton resetButton;
    private JButton settingsButton;
    private int workTime;
    private int breakTime;
    private int timeLeft;
    private boolean isWorkTime;

    public PomodoroTimer() {
        // Configuración de la ventana
        setTitle("Pomodoro Timer");
        setSize(300, 250);
        setLocationRelativeTo(null);
        setDefaultCloseOperation(EXIT_ON_CLOSE);

        // Configuración del temporizador
        workTime = 25 * 60; // 25 minutos en segundos
        breakTime = 5 * 60; // 5 minutos en segundos
        timeLeft = workTime;
        isWorkTime = true;
        timer = new Timer(1000, new TimerListener());

        // Configuración de los componentes de la interfaz
        timeLabel = new JLabel(formatTime(timeLeft), SwingConstants.CENTER);
        timeLabel.setFont(new Font("Arial", Font.BOLD, 36));
        startButton = new JButton("Start");
        startButton.addActionListener(new StartListener());
        resetButton = new JButton("Reset");
        resetButton.addActionListener(new ResetListener());
        settingsButton = new JButton("Settings");
        settingsButton.addActionListener(new SettingsListener());

        // Configuración del panel
        JPanel panel = new JPanel(new GridLayout(2, 2, 10, 10));
        panel.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
        panel.add(new JLabel(""));
        panel.add(new JLabel(""));
        panel.add(startButton);
        panel.add(resetButton);
        add(timeLabel, BorderLayout.CENTER);
        add(panel, BorderLayout.SOUTH);
        add(settingsButton, BorderLayout.NORTH);
    }

    // Formato de tiempo para mostrar en la interfaz
    private String formatTime(int seconds) {
        int minutes = seconds / 60;
        int secs = seconds % 60;
        return String.format("%02d:%02d", minutes, secs);
    }

    // Clase oyente para el temporizador
    private class TimerListener implements ActionListener {
        public void actionPerformed(ActionEvent e) {
            timeLeft--;
            if (timeLeft == 0) {
                timer.stop();
                if (isWorkTime) {
                    isWorkTime = false;
                    timeLeft = breakTime;
                } else {
                    isWorkTime = true;
                    timeLeft = workTime;
                }
                // Aquí puedes agregar una alarma o notificación
                JOptionPane.showMessageDialog(null, isWorkTime ? "¡Hora de trabajar de nuevo!" : "¡Descanso terminado!");
                timeLabel.setText(formatTime(timeLeft));
            } else {
                timeLabel.setText(formatTime(timeLeft));
            }
        }
    }

    // Clase oyente para el botón de inicio
    private class StartListener implements ActionListener {
        public void actionPerformed(ActionEvent e) {
            timer.start();
        }
    }

    // Clase oyente para el botón de reset
    private class ResetListener implements ActionListener {
        public void actionPerformed(ActionEvent e) {
            timer.stop();
            timeLeft = workTime;
            isWorkTime = true;
            timeLabel.setText(formatTime(timeLeft));
        }
    }
     // Clase oyente para el botón de ajustes
     private class SettingsListener implements ActionListener {
        public void actionPerformed(ActionEvent e) {
            // Crear un diálogo emergente para que el usuario ingrese los intervalos de tiempo
            JTextField workTimeField = new JTextField(String.valueOf(workTime / 60), 5);
            JTextField breakTimeField = new JTextField(String.valueOf(breakTime / 60), 5);
            JPanel panel = new JPanel(new GridLayout(2, 2));
            panel.add(new JLabel("Tiempo de trabajo (min):"));
            panel.add(workTimeField);
            panel.add(new JLabel("Tiempo de descanso (min):"));
            panel.add(breakTimeField);
            int result = JOptionPane.showConfirmDialog(null, panel, "Ajustes", JOptionPane.OK_CANCEL_OPTION);
            if (result == JOptionPane.OK_OPTION) {
                // Actualizar los valores de los intervalos de tiempo
                try {
                    int newWorkTime = Integer.parseInt(workTimeField.getText()) * 60;
                    int newBreakTime = Integer.parseInt(breakTimeField.getText()) * 60;
                    if (newWorkTime > 0 && newBreakTime > 0) {
                        workTime = newWorkTime;
                        breakTime = newBreakTime;
                        resetButton.doClick();
                    } else {
                        JOptionPane.showMessageDialog(null, "Los intervalos de tiempo deben ser mayores que cero.");
                    }
                } catch (NumberFormatException ex) {
                    JOptionPane.showMessageDialog(null, "Ingresa números válidos.");
                }
            }
        }
    }
    
    public static void main(String[] args) {
        PomodoroTimer timer = new PomodoroTimer();
        timer.setVisible(true);
    }
}