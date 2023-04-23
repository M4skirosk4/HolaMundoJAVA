package Pom2;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

public class PomodoroTimer extends JFrame {
    private Timer timer;
    private JLabel timeLabel;
    private JButton startButton;
    private JButton resetButton;
    private int timeLeft;
    private boolean isWorkTime;

    public PomodoroTimer() {
        // Configuración de la ventana
        setTitle("Pomodoro Timer");
        setSize(300, 150);
        setLocationRelativeTo(null);
        setDefaultCloseOperation(EXIT_ON_CLOSE);

        // Configuración del temporizador
        timeLeft = 25 * 60; // 25 minutos en segundos
        isWorkTime = true;
        timer = new Timer(1000, new TimerListener());

        // Configuración de los componentes de la interfaz
        timeLabel = new JLabel(formatTime(timeLeft), SwingConstants.CENTER);
        timeLabel.setFont(new Font("Arial", Font.BOLD, 36));
        startButton = new JButton("Start");
        startButton.addActionListener(new StartListener());
        resetButton = new JButton("Reset");
        resetButton.addActionListener(new ResetListener());

        // Configuración del panel
        JPanel panel = new JPanel(new GridLayout(2, 2, 10, 10));
        panel.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
        panel.add(new JLabel(""));
        panel.add(new JLabel(""));
        panel.add(startButton);
        panel.add(resetButton);
        add(timeLabel, BorderLayout.CENTER);
        add(panel, BorderLayout.SOUTH);
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
                    timeLeft = 5 * 60; // 5 minutos de descanso
                } else {
                    isWorkTime = true;
                    timeLeft = 25 * 60; // 25 minutos de trabajo
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
            timeLeft = 25 * 60;
            isWorkTime = true;
            timeLabel.setText(formatTime(timeLeft));
        }
    }

    public static void main(String[] args) {
        PomodoroTimer timer = new PomodoroTimer();
        timer.setVisible(true);
    }
}